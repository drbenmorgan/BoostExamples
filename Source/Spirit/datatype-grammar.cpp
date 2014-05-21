// datatype-grammar - simple tests of spirit::qi grammar for LPC
// properties datatype
//
// A datatype entry in properties is defined by the grammar
//
//   <identifier> ::= <c-style-identifier>
//   <datatype> ::= <identifier> ":" <type> "=" <value> <EOL>
//
// Here's an initial issue - the grammar of <value> depends on that
// of <type>. For example, we can't do "foo : real = blab"
// Better to break down individually(??), i.e.
//
//   <datatype> ::= <identifier> ":" <assignment> <EOL>
//   <assignment> ::= <int-assignment> | <real-assignment> | ...
//   <int-assignment> ::= "int" "=" <integer>
//
// See also the "Nabialek Trick":
//
// http://boost-spirit.com/home/articles/qi-example/nabialek-trick/
//
// which appears to address exactly this problem. This does work
// in the current solution, but care is needed with attributes
//
// Another issue: all the basic types have list equivalents, e.g
//
// foo : int = 1
// bar : int[2] = 10 20
//
// C-style array notation is used, with elements separated by whitespace.
//
// Note also that recent versions of the properties grammar allow an
// additional "dimension" specifier for reals and strings
//
//  foo : real as <dimension> = 2.0 <unit>
//
// Here, the valid parses are more difficult
// - specifying a dimension is only valid for reals and strings
// - there must be a unit if dimension is specified for reals
// - no unit for strings with dimension
//
// Need to check: can arrays of reals/strings have dimension
// Better, probably, to have "string as path" as type "path"
// because thats what it really is. One could also do the same
// for "real as X" by just having "X".
//
// What about nested properties? We can build up a hierarchy
// just as with JSON:
//
// foo : pset = {
//   bar : int = 1,
//   baz : real = 3.14
//   bob : pset = {
//     rob : bitset = 0xffffff
//     }
//   }
//
// That's a list of properties though!
// A "propertyset" is a set of properties, and a property may contain
// a propertyset!
//
// Will need to think about directives as well, main use case
// is the property description, e.g.
//
// @description "description of foo"
// foo : int = 1
//
// Copyright (c) 2014 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2014 by The University of Warwick
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Standard Library
#include <iostream>
#include <iterator>
#include <algorithm>
// Third Party
// - Boost
#include "boost/variant.hpp"
#include "boost/dynamic_bitset.hpp"
#include "boost/fusion/include/adapt_struct.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_char.hpp>
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
namespace phx = boost::phoenix;

#include "boost/lexical_cast.hpp"
// This Project

//----------------------------------------------------------------------
// Basic types
//
namespace warwick {
  struct Property;
}

std::ostream& operator<<(std::ostream& os, const warwick::Property& p);

namespace warwick {
struct Property {
  typedef std::string key_type;
  typedef boost::variant<int,
                         double,
                         bool,
                         std::string,
                         boost::dynamic_bitset<>,
                         std::vector<int>,
                         std::vector<double>,
                         std::vector<std::string>,
                         boost::recursive_wrapper<std::vector<warwick::Property> > > value_type;
  key_type Key;
  value_type Value;

  /// Visitor that outputs the property value to a given ostream
  struct ostream_visitor : public boost::static_visitor<void> {
  public:
    ostream_visitor(std::ostream& os) : os_(os) {}
    
    std::ostream& os_;
  
    template <typename T>
    void operator()(const T& arg) const {
      os_ << arg;
    }
    
    template<typename U>
    void operator()(const std::vector<U>& arg) const {
      typename std::vector<U>::const_iterator iter = arg.begin();
      typename std::vector<U>::const_iterator end = arg.end();
      while (iter != end) {
        os_ << *iter << ",";
        ++iter;
      }
      // Copy doesn't work...
      //std::copy(arg.begin(), arg.end(), std::ostream_iterator<U>(os_,","));
    }
  };
};

} // namespace warwick

// NB: using a struct for convenience, later, can use ADAPT_ADT for getting/setting
// attributes
BOOST_FUSION_ADAPT_STRUCT(
    warwick::Property,
    (warwick::Property::key_type, Key)
    (warwick::Property::value_type, Value)
    )

// Output streams for convenience
std::ostream& operator<<(std::ostream& os, const warwick::Property& p) {
  // need a vistor for sequence types
  os << "[" << "key: " << p.Key << "," << "value[" << p.Value.which() << "]: ";
  boost::apply_visitor(warwick::Property::ostream_visitor(os),p.Value);
  os << "]";
  return os;
}


//----------------------------------------------------------------------
/// Convert hex string, e.g. 0xF to supplied type
template <typename T>
struct HexTo {
  T value;
  operator T() const {
    return value;
  }
  
  friend std::istream& operator>>(std::istream& in, HexTo& out) {
    in >> std::hex >> out.value;
    return in;
  }
};

/// Convert a string representing a bitset in hexadecimal form to
/// a boost dynamic_bitset. It's done in this odd way because
/// we use a conversion via unsigned long, and we have to
/// construct the bitset with number of bits *and* the value.
struct HexConverterImpl {
  template <typename T1>
  struct result {typedef boost::dynamic_bitset<> type;};
  
  boost::dynamic_bitset<> operator()(const std::string& s) const
  {
    size_t nxdigits(s.size() - 2);
    unsigned long value(boost::lexical_cast<HexTo<unsigned long> >(s));
    return boost::dynamic_bitset<>(nxdigits*4,value);
  }
};

/// Create a phoenix wrapper of the hex converter
phx::function<HexConverterImpl> PHXHexConverter;


namespace warwick {
// Try to make grammar of scalars/sequence easier - implies no
// backward compatibility
//
// alpha        <- [a-zA-Z]
// alphanumeric <- (alpha / [0-9])
// identifier   <- alpha / (alphanumeric | '_')*
// integer      <- 'int' EQUALS (int_ / intsequence)
// intsequence  <- '[' int_ (',' int_)* ']'
// real         <- 'real' EQUALS (real_ / realsequence)
// bool         <- 'bool' EQUALS bool_
// string       <- 'string' EQUALS (string_ / stringsequence)
// property     <- identifier ':' (integer / real / bool / string)
//
template <typename Iterator>
struct PropertyParser : qi::grammar<Iterator, warwick::Property(), ascii::space_type> {
  PropertyParser() : PropertyParser::base_type(property) {
    identifier %= qi::alpha >> *(qi::alnum | qi::char_('_'));
    quotedstring %= qi::lexeme['"' >> +(qi::char_ - '"') >> '"'];

    integer %= qi::int_ | ('[' > qi::int_ % ',' > ']');
    typedvalue.add("int", &integer);

    real %= qi::double_ | ('[' > qi::double_ % ',' > ']');
    typedvalue.add("real", &real);

    string %= quotedstring | ('[' > quotedstring % ',' > ']');
    typedvalue.add("string", &string);

    boolean %= qi::bool_;
    typedvalue.add("bool", &boolean);

    // bitsets can be synthesized as a sequence of 0s and 1s
    // or as hexadecimal. We parse as strings and then use
    // phoenix conversion specialized for hex/bit notation.
    // This split is used so that attributes have correct
    // compatibility:
    // http://boost.2283326.n4.nabble.com/Spirit-Qi-variant-with-std-string-td2715777.html
    // NB: I'm not a spirit expert, so likely this can be done better!
    binary_bitset %= qi::lexeme[qi::repeat(1,64)[qi::char_("01")]];
    
    hex_bitset %= qi::lexeme[qi::string("0x") > qi::repeat(1,16)[ascii::xdigit]];
    
    bitset = hex_bitset[
                qi::_val = PHXHexConverter(qi::_1)
              ] 
              | 
              binary_bitset[
                qi::_val = phx::construct<boost::dynamic_bitset<> > (qi::_1)
              ];
    typedvalue.add("bitset", &bitset);

    // A property can contain a property set
    property_set = ('{' > property % ',' > '}')[qi::_val = qi::_1];
    typedvalue.add("pset", &property_set);
    
    // Assignment uses rule appropriate to parsed type keyword
    typedassignment = qi::omit[typedvalue[qi::_a = qi::_1]] > '=' > qi::lazy(*qi::_a);

    // Now the actual grammar of the property
    property %= identifier > ':' > typedassignment;

    // Because we use expectations, provide simple error handler
    qi::on_error<qi::fail>(property,
                           std::cout << phx::val("Error! Expecting ")
                           << qi::labels::_4
                           <<std::endl
                           );
  }

  typedef qi::rule<Iterator, warwick::Property::value_type(), ascii::space_type> value_rule_t;

  qi::rule<Iterator, std::string()> identifier;
  qi::rule<Iterator, std::string(), ascii::space_type> quotedstring;
  value_rule_t integer;
  value_rule_t real;
  value_rule_t string;
  value_rule_t boolean;

  qi::rule<Iterator, std::string()> binary_bitset;
  qi::rule<Iterator, std::string()> hex_bitset;
  value_rule_t bitset;

  value_rule_t property_set;
  
  qi::symbols<char, value_rule_t*> typedvalue;

  qi::rule<Iterator,warwick::Property::value_type(), ascii::space_type,
      qi::locals<value_rule_t*> > typedassignment;

  qi::rule<Iterator, warwick::Property(), ascii::space_type> property;
};



} // namespace warwick


//----------------------------------------------------------------------
// Parse
//
void parse_string(const std::string& input) {
  std::string::const_iterator first(input.begin());
  std::string::const_iterator last(input.end());

  warwick::PropertyParser<std::string::const_iterator> g;
  warwick::Property p;
  // apply eoi after property parser because here there should be
  // no trailing input
  bool result = qi::phrase_parse(first,
      last,
      g >> qi::eoi,
      ascii::space,
      p
      );

  if (!result) {
    std::cout << "Failed to parse \"" << input << "\"" << std::endl;
    return;
  }
  if (first != last) {
    std::cout << "No complete parse of \"" << input << "\"" << std::endl;
    std::cout << "Dangling input:" << std::endl;
    std::copy(first,last,std::ostream_iterator<const char>(std::cout));
    std::cout << std::endl;

    return;
  }

  std::cout << "Successful parse of \"" << input << "\"" << std::endl;
  std::cout << "Property = " << p << std::endl;

}





int main(int argc, const char *argv[])
{
  std::cout << "[datatype-grammar] qi parsing of properties\n";
  std::cout << "Type [q or Q] to quit\n\n";

  std::string input;

  std::cout << ">>> ";

  while (std::getline(std::cin, input)) {
    if (input.empty() || input[0] == 'q' || input[0] == 'Q') break;
    parse_string(input);
    std::cout << ">>> ";
  }

  std::cout << "[quit]\n";

  return 0;
}
