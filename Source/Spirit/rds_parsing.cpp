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

struct Property {
  std::string id;
  PValue value;
};

BOOST_FUSION_ADAPT_STRUCT(
    Property,
    (std::string, id)
    (PValue, value)
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
  os << "[" << p.id << ", ";
  boost::apply_visitor(ostream_visitor(os),p.value);
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


template <typename Iterator, typename Skipper>
struct PropertyParser : qi::grammar<Iterator, Property(), Skipper> {
  PropertyParser() : PropertyParser::base_type(property) {
    property %= qi::omit[-description] >> (identifier > ':' > assignment);

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

    BOOST_SPIRIT_DEBUG_NODE(property);
    BOOST_SPIRIT_DEBUG_NODE(node);
    BOOST_SPIRIT_DEBUG_NODE(tree);
  }

  typedef qi::rule<Iterator, PValue(), Skipper> value_rule_t;
  typedef qi::rule<Iterator, PList(), Skipper> list_rule_t;

  qi::rule<Iterator, Property(), Skipper> property;
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

template <typename Iterator, typename Skipper>
struct PropertyListParser : qi::grammar<Iterator, PList(), Skipper> {
  PropertyListParser() : PropertyListParser::base_type(document) {
    document %= *property;
    BOOST_SPIRIT_DEBUG_NODE(document);
  }

  qi::rule<Iterator, PList(), Skipper> document;
  PropertyParser<Iterator, Skipper> property;
};

int main(int argc, const char *argv[])
{
  std::ifstream input(argv[1]);
  input.unsetf(std::ios::skipws);

  boost::spirit::istream_iterator first(input);
  boost::spirit::istream_iterator last;

  PropertyListParser<boost::spirit::istream_iterator, qi::space_type> g;
  PList p;
  bool result = qi::phrase_parse(first, last, g > qi::eoi, qi::space, p);

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

