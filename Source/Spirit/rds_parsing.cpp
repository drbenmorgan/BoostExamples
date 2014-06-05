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
#include <boost/fusion/include/std_pair.hpp>
namespace qi = boost::spirit::qi;

struct Property;
typedef std::vector<Property> PList;
typedef boost::variant<int, boost::recursive_wrapper<PList> > PValue;

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
  void operator()(const T& arg) const {os_ << "int(" << arg << ")";}

  template<typename U>
  void operator()(const std::vector<U>& arg) const {
    typename std::vector<U>::const_iterator iter = arg.begin();
    typename std::vector<U>::const_iterator end = arg.end();
    os_ << "plist{";
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
    property %= identifier > '=' > value;
    identifier %= qi::alpha >> *(qi::alnum | qi::char_('_'));
    value %= qi::int_ | plist;
    plist %= '{' > property % ',' > '}';
    BOOST_SPIRIT_DEBUG_NODE(property);
  }

  qi::rule<Iterator, Property(), Skipper> property;
  qi::rule<Iterator, std::string(), Skipper> identifier;
  qi::rule<Iterator, PValue(), Skipper> value;
  qi::rule<Iterator, PList(), Skipper> plist;
};

template <typename Iterator, typename Skipper>
struct PropertyListParser : qi::grammar<Iterator, PList(), Skipper> {
  PropertyListParser() : PropertyListParser::base_type(document) {
    document %= *property; // % qi::char_('\n');
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
  bool result = qi::phrase_parse(first, last, g, qi::space, p);

  if (result) {
    std::cout << "good parse" << std::endl;
    std::cout << p << std::endl;
  } else {
    std::cerr << "bad parse" << std::endl;
  }

  return 0;
}

