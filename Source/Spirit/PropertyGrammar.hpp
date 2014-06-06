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
#define BOOST_SPIRIT_DEBUG
#include "boost/fusion/include/adapt_struct.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_char.hpp>

// This Project
#include "Property.hpp"
#include "BitsetGrammar.hpp"

// NB: using a struct for convenience, later, can use ADAPT_ADT for getting/setting
// attributes
BOOST_FUSION_ADAPT_STRUCT(
    warwick::Property,
    (warwick::Property::key_type, Key)
    (warwick::Property::value_type, Value)
    )


//----------------------------------------------------------------------

namespace warwick {
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phx = boost::phoenix;

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
struct PropertyParser : qi::grammar<Iterator, warwick::Property(), qi::blank_type> {
  PropertyParser() : PropertyParser::base_type(property) {
    identifier %= qi::alpha >> *(qi::alnum | qi::char_('_'));
    quotedstring %= qi::lexeme['"' >> +(qi::char_ - '"') >> '"'];

    integer %= qi::int_
               | qi::skip(qi::space)[
                   '[' > qi::int_ % "," > ']'
                  ];
    typedvalue.add("int", &integer);

    real %= qi::double_
            | qi::skip(qi::space)[
                '[' > qi::double_ % "," > ']'
              ];
    typedvalue.add("real", &real);

    string %= quotedstring
              | qi::skip(qi::space)[
                  '[' > quotedstring % ',' > ']'
                ];
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

    BOOST_SPIRIT_DEBUG_NODE(property);
    BOOST_SPIRIT_DEBUG_NODE(typedassignment);
    // Because we use expectations, provide simple error handler
    qi::on_error<qi::fail>(property,
                           std::cout << phx::val("Error! Expecting ")
                           << qi::labels::_4
                           <<std::endl
                           );
  }

  typedef qi::rule<Iterator, warwick::Property::value_type(), qi::blank_type> value_rule_t;

  qi::rule<Iterator, std::string()> identifier;
  qi::rule<Iterator, std::string()> quotedstring;
  value_rule_t integer;
  value_rule_t real;
  value_rule_t string;
  value_rule_t boolean;
  BoostExamples::BitsetParser<Iterator> bitset_rule;
  value_rule_t bitset;

  value_rule_t property_set;

  qi::symbols<char, value_rule_t*> typedvalue;

  qi::rule<Iterator,warwick::Property::value_type(), qi::blank_type,
      qi::locals<value_rule_t*> > typedassignment;

  qi::rule<Iterator, warwick::Property(), qi::blank_type> property;
};


//----------------------------------------------------------------------
// A properties "document" is zero or more properties.
// For now we just stuff these into a vector
//
template <typename Iterator>
struct PropertyListGrammar :
    public qi::grammar<Iterator, warwick::PropertyList(), qi::blank_type> {
  PropertyListGrammar() : PropertyListGrammar::base_type(document) {
    document %= qi::omit[*qi::space]
                >> property_rule % +qi::char_("\n")
                >> qi::omit[*qi::space]
                > qi::eoi;
    BOOST_SPIRIT_DEBUG_NODE(document);
  }

  PropertyParser<Iterator> property_rule;
  qi::rule<Iterator, warwick::PropertyList(), qi::blank_type> document;
};

} // namespace warwick


