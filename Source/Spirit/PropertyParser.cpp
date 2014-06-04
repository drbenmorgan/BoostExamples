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


bool parse_string(const std::string& input) {
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
    return false;
  }
  if (first != last) {
    std::cout << "No complete parse of \"" << input << "\"" << std::endl;
    std::cout << "Dangling input:" << std::endl;
    std::copy(first,last,std::ostream_iterator<const char>(std::cout));
    std::cout << std::endl;

    return false;
  }

  std::cout << "Successful parse of \"" << input << "\"" << std::endl;
  std::cout << "Property = " << p << std::endl;

  return true;
}

