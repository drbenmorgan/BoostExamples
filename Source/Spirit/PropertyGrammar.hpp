// PropertyGrammar - simple tests of spirit::qi grammar for LPC
// properties/multiproperties datatype
//
// At the most basic level, "properties" are key-value pairs with type
// A "property" is defined by the grammar
//
//   <identifier> ::= <c-style-identifier>
//   <typename> ::= int | real | bool | ...
//   <datatype> ::= <identifier> ":" <typename> "=" <value>
//
// From this, a "properties" object is the grammar
//
//   <properties> ::= +(<datatype> <EOL>)
//
// A secondary structure is "multiproperties" which allows separate
// groups of properties to be isolated into "sections"
//
// <section> ::= '[' <idinfo> ']'
// <multiproperties> ::= +(<section> <properties>)
//
// Thus "multiproperties" is nothing more than a one level
// hierarchy of "properties". It therefore makes sense to try
// and make "properties" hierarchical - that is,
//
// <value> ::= <integer> | <double> | ... | <properties>
//


// Issues with the properties grammar
// ----------------------------------
// Typesafe parsing
// ----------------
// To get "typesafe" parsing, the parser for <value> must depend on
// the result of the <typename> parser. Could represent this as
//
// <intassignment> ::= 'int' '=' <integer>
// ...
// <datatype> ::= <identifier> ':' (<intassignment> | <realassignment>)
//
// Problem with this is the number of datatypes, especially if it
// gets extended. Potentially better way to do this is the
// "Nabialek Trick":
//
// http://boost-spirit.com/home/articles/qi-example/nabialek-trick/
//
// which is implemented in the parser. Only thing to watch with this
// is how spirit's attributes work.
//
// Descriptions
// ------------
// Entries can have descriptions:
//
//  #@description "foo bars"
//  foo : int = 1
//
// Not implemented yet.
//
// Array specification
// -------------------
// All types have array equivalents, e.g
//
// foo : int = 1
// bar : int[2] = 10 20
//
// C-style array notation is used, with elements separated by whitespace.
// This is only needed because of the handwritten parser. With a smarter
// parser, we don't need this, but, then require a better syntax for
// arrays (because it's a bit tricky to distinguish "1" from "1 2").
// This implementation replaces the LPC syntax for arrays with a
// Python/JSON style list notation:
//
// foo : int = 1
// foo : int = [1,2,3 , 4 ]
//
// Hierarchical Structure
// ----------------------
// As noted above, no reason not to merge "multiproperties" functionality
// into "properties" to create a full hierarchical system.
// All that is needed is a recursive grammar:
//
//   <typename> ::= int | real | bool | ... | properties
//   <value> ::= <integer> | <real> | ... | <properties>
//   <property> ::= <identifier> ":" <typename> "=" <value>
//   <properties> ::= +<property>
//
// For example, we could use a Python dict/JSON object style notation
//
// foo : properties = {
//   bar : int = 1,
//   baz : bool = true,
//   bob : properties {
//    fu : string = "bar"
//   },
//   rob : real = [3.14, 4.13]
// }
//
// It's this structure that the grammar parses below.
// Some thought is still needed on how a "document" is defined, e.g.
// pure hierarchy as xml, or '*any' as in JSON.
//
// Property Traits
// ---------------
// Latest versions of properties provide a very basic form of "traits"
// for property entries. For example, a real can have a unit, with the
// syntax
//
//  foo : real as <dimension> = 2.0 <unit>
//
// similarly for strings, which can be paths
//
//  foo : string as path = "/path/to"
//
// In the latter case, it may be better to just specify a new type
// "path" with appropriate parsing (albeit there's little difference,
// and any path operations are the job of the path object, not the parser).
//
// Similar syntax may be added, so this needs to be watched for.
// In the dimension case, it may be easier simply to have the parser
// look for units following the number.
//
//
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
/// It is designed to be used with boost lexical cast
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


namespace warwick {
// Bitset parser
// - Parse bitsets represented by strings in the form of:
//   i) One or more binary digits, e.g. "0101"
//   ii) A "hexadecimal" word, e.g. 0xffff
// Because we want to support a variable number of bits
// (though not completely unbounded), parser should
// have an attribute of boost::dynamic_bitset
// Parser should not skip, because forms should never have
// whitespace
template <typename Iterator>
  class BitsetParser : public qi::grammar<Iterator, boost::dynamic_bitset<>()> {
  public:
    BitsetParser() : BitsetParser::base_type(bitset) {
      // bitsets can be synthesized as a sequence of 0s and 1s
      // or as hexadecimal. We parse as strings and then use
      // phoenix conversion specialized for hex/bit notation.
      // This split is used so that attributes have correct
      // compatibility:
      // http://boost.2283326.n4.nabble.com/Spirit-Qi-variant-with-std-string-td2715777.html
      // NB: I'm not a spirit expert, so likely this can be done better!
      bitset = hex_bitset[qi::_val = HexConverter_(qi::_1)]
               |
               binary_bitset[qi::_val = phx::construct<boost::dynamic_bitset<> >(qi::_1)];

      binary_bitset %= qi::repeat(1,64)[qi::char_("01")];

      hex_bitset %= qi::string("0x") > qi::repeat(1,16)[ascii::xdigit];

    }

  private:
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

  private:
    typedef qi::rule<Iterator,boost::dynamic_bitset<>()> bitset_rule_t;
    typedef qi::rule<Iterator,std::string()> bitset_string_t;
    bitset_string_t binary_bitset;
    bitset_string_t hex_bitset;
    bitset_rule_t bitset;
    phx::function<HexConverterImpl> HexConverter_;
  };

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

    // TODO : check why we have to do this two level definition
    // i.e., can't just assign bitset to the BitsetParser instance
    // *suspect* it's because we're using pointers-to-rules
    bitset %= bitset_rule;
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
  BitsetParser<Iterator> bitset_rule;
  value_rule_t bitset;

  value_rule_t property_set;

  qi::symbols<char, value_rule_t*> typedvalue;

  qi::rule<Iterator,warwick::Property::value_type(), ascii::space_type,
      qi::locals<value_rule_t*> > typedassignment;

  qi::rule<Iterator, warwick::Property(), ascii::space_type> property;
};



} // namespace warwick


