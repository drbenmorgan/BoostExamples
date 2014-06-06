// - implementation of property parser frontends
//
// Copyright (c) 2014 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2014 by The University of Warwick
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Ourselves
#include "PropertyParser.hpp"

// Standard Library

// Third Party
// - A

// This Project
#include "PropertyGrammar.hpp"
#include <boost/spirit/include/support_istream_iterator.hpp>

bool parse_string(const std::string& input, warwick::Property& output) {
  std::string::const_iterator first(input.begin());
  std::string::const_iterator last(input.end());

  warwick::PropertyParser<std::string::const_iterator> g;
  // apply eoi after property parser because here there should be
  // no trailing input
  bool result = qi::phrase_parse(first,
      last,
      g > qi::eoi,
      qi::blank,
      output
      );

  // Handle incomplete parse
  if (first != last) {
    std::cerr << "No complete parse of \"" << input << "\"" << std::endl;
    std::cerr << "Dangling input:" << std::endl;
    std::copy(first,last,std::ostream_iterator<const char>(std::cerr));
    std::cerr << std::endl;
    return false;
  }

  return result;
}


bool parse_istream(std::istream& input, warwick::Property& output) {
  boost::spirit::istream_iterator first(input);
  boost::spirit::istream_iterator last;

  warwick::PropertyParser<boost::spirit::istream_iterator> g;
  // apply eoi after property parser because here there should be
  // no trailing input
  bool result = qi::phrase_parse(first,
      last,
      g > qi::eoi,
      qi::blank,
      output
      );

  // Handle incomplete parse
  if (first != last) {
    std::cerr << "No complete parse of \"" << input << "\"" << std::endl;
    std::cerr << "Dangling input:" << std::endl;
    std::copy(first,last,std::ostream_iterator<const char>(std::cerr));
    std::cerr << std::endl;
    return false;
  }

  return result;
}

bool parse_document(std::istream& input, warwick::PropertyList& output) {
  boost::spirit::istream_iterator first(input);
  boost::spirit::istream_iterator last;

  warwick::PropertyListGrammar<boost::spirit::istream_iterator> g;
  // apply eoi after property parser because here there should be
  // no trailing input
  bool result = qi::phrase_parse(first,
      last,
      g,
      qi::blank,
      output
      );

  // Handle incomplete parse
  if (first != last) {
    std::cerr << "No complete parse of \"" << input << "\"" << std::endl;
    std::cerr << "Dangling input:" << std::endl;
    //std::copy(first,last,std::ostream_iterator<const char>(std::cerr));
    std::cerr << std::endl;
    return false;
  }

  return result;
}

