// rds_parsing - simple parsing of a filesystem like data structure,
// represented in text format like
//
// ... document start ...
// key_1 = value_1
// key_2 = value_2
// key_3 = {
//   key_4 = value_4,
//   key_5 = value_5
//   }
//
// So in terms of a grammar, this is
//
// document ::= pair % '\n'
// pair ::= key '=' value
// value ::= int | '{' document '}'
//
//
#define BOOST_SPIRIT_DEBUG
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <boost/foreach.hpp>

#include <boost/variant.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/std_pair.hpp>
namespace qi = boost::spirit::qi;

struct Property;
typedef std::vector<Property> PList;
typedef boost::variant<int,
                       double,
                       bool,
                       std::string,
                       std::vector<int>,
                       std::vector<double>,
                       std::vector<std::string>,
                       boost::recursive_wrapper<PList> > PValue;

/// We use this awkward split because the parsing of the optional
/// description string means, I think, that we end up with a
/// fusion sequence of "tuple<Description,tuple<Id,Value> >"
/// May therefore wish to rewrite rules to construct object
/// by hand. It does appear that in this case the grammar is somewhat
/// awakward to directly adapt. Internally, we'd really like mappings
/// id -> (description, value)
/// or
/// (id, description) -> value
/// or just
/// id, description, value
/// We *could just have a struct of
/// description, id, value, but then we'd certainly need by hand
/// construction using at least a phoenix construct function.
//
// TODO : Investigate these options, and whether the "description"
// is needed at all internally (probably is...)
struct PAtom {
  std::string id;
  PValue value;
};

struct Property {
  std::string desc;
  PAtom value;
};


BOOST_FUSION_ADAPT_STRUCT(
    PAtom,
    (std::string, id)
    (PValue, value)
    )

BOOST_FUSION_ADAPT_STRUCT(
    Property,
    (std::string, desc)
    (PAtom, value)
    )



/// Visitor that outputs the property value to a given ostream
struct ostream_visitor : public boost::static_visitor<void> {
 public:
  ostream_visitor(std::ostream& os) : os_(os) {}
  std::ostream& os_;

  template <typename T>
  void operator()(const T& arg) const {os_ << "value(" << arg << ")";}

  template<typename U>
  void operator()(const std::vector<U>& arg) const {
    typename std::vector<U>::const_iterator iter = arg.begin();
    typename std::vector<U>::const_iterator end = arg.end();
    os_ << "list{";
    while (iter != end) {
      os_ << *iter << ",";
      ++iter;
    }
    os_ << "}";
  }
};

// Output streams for convenience
std::ostream& operator<<(std::ostream& os, const Property& p) {
  // need a vistor for sequence types
  os << "[" << p.value.id << ", "
     << "\"" << p.desc << "\", ";
  boost::apply_visitor(ostream_visitor(os),p.value.value);
  os << "]";
  return os;
}

std::ostream& operator<<(std::ostream& os, const PList& d) {
  PList::const_iterator iter = d.begin();
  PList::const_iterator end = d.end();
  while (iter != end) {
    os << *iter << "\n";
    ++iter;
  }
  return os;
}


/// PropertyParser : parse a single property
template <typename Iterator, typename Skipper>
struct PropertyParser : qi::grammar<Iterator, Property(), Skipper> {
  PropertyParser() : PropertyParser::base_type(property) {
    // NB: this is using the same form so descriptions can truly
    // be optional - awkwardness *probably* arises from
    // combination of sequence and expectation ops so that the
    // attribute thats exposed is tuple<Desc, tuple<Id, Value> >
    // hence the split into two fusion adapted structs
    property %= -description >> (identifier > ':' > assignment);

    description %= "@description" > quotedstring;
    quotedstring %= qi::lexeme['"' >> +(qi::char_ - '"') >> '"'];
    identifier %= qi::alpha >> *(qi::alnum | qi::char_('_'));

    assignment %= node | tree;

    // Node types - order in which they are added shouldn't matter...
    node %= qi::omit[nodetypes[qi::_a = qi::_1]] > '=' > qi::lazy(*qi::_a);

    // Strict integer parsing is needed as qi::int_ will parse
    // "1.23" as "1" and leave the ".23" dangling. Though that will
    // subsequently result in a parse fail, construct an explicit, strict
    // integer parser so the error can be better localized.
    strictint_ %= qi::int_ >> !qi::double_;
    intnode %= strictint_ | '[' > strictint_ % ',' > ']';
    nodetypes.add("int", &intnode);

    realnode %= qi::double_ | '[' > qi::double_ % ',' > ']';
    nodetypes.add("real", &realnode);

    boolnode %= qi::bool_;
    nodetypes.add("bool", &boolnode);

    stringnode %= quotedstring | '[' > quotedstring % ',' > ']';
    nodetypes.add("string", &stringnode);

    // tree of nodes isn't typed because grammar is distinct...
    tree %= '{' > property % ',' > '}';

    BOOST_SPIRIT_DEBUG_NODES(
        (property)
        (description)
        (identifier)
        (assignment)
        );
  }

  typedef qi::rule<Iterator, PValue(), Skipper> value_rule_t;
  typedef qi::rule<Iterator, PList(), Skipper> list_rule_t;

  qi::rule<Iterator, Property(), Skipper> property;
  // Identifier should be whole word, so don't skip
  qi::rule<Iterator, std::string()> identifier;
  qi::rule<Iterator, std::string(), Skipper> description;
  qi::rule<Iterator, std::string(), Skipper> quotedstring;
  value_rule_t assignment;
  list_rule_t tree;

  qi::rule<Iterator, PValue(), Skipper, qi::locals<value_rule_t*> > node;
  qi::symbols<char, value_rule_t*> nodetypes;
  qi::rule<Iterator, int()> strictint_;
  value_rule_t intnode;
  value_rule_t realnode;
  value_rule_t boolnode;
  value_rule_t stringnode;
};

/// PropertyDocumentParser : parse a properties document
template <typename Iterator, typename Skipper>
struct PropertyDocumentParser : qi::grammar<Iterator, PList(), Skipper> {
  PropertyDocumentParser() : PropertyDocumentParser::base_type(document) {
    document %= *property;
    BOOST_SPIRIT_DEBUG_NODE(document);
  }

  qi::rule<Iterator, PList(), Skipper> document;
  PropertyParser<Iterator, Skipper> property;
};

/// PropertySkipper : custom skipper that skips whitespace and
/// Bash/Python style comments
template <typename Iterator>
struct PropertySkipper : public qi::grammar<Iterator> {
  PropertySkipper() : PropertySkipper::base_type(skip) {
    skip %= qi::space | comment;
    comment %= qi::lexeme['#' >> *(qi::char_ - qi::eol) >> qi::eol];
  }

  qi::rule<Iterator> skip;
  qi::rule<Iterator> comment;
};

int main(int argc, const char *argv[])
{
  std::ifstream input(argv[1]);
  input.unsetf(std::ios::skipws);

  typedef boost::spirit::istream_iterator Iterator;
  typedef PropertySkipper<Iterator> Skipper;

  Iterator first(input);
  Iterator last;
  PropertyDocumentParser<Iterator,Skipper> g;
  PList p;

  bool result = qi::phrase_parse(
      first,
      last,
      g > qi::eoi,
      Skipper(),
      p);

  if (result) {
    std::cout << "good parse" << std::endl;
    std::cout << p << std::endl;

    if(first != last) {
      std::cerr << "... but with trailing input..." << std::endl;
    }
  } else {
    std::cerr << "bad parse" << std::endl;
  }

  return 0;
}

