// property-filereader - simple tests of spirit::qi grammar for LPC
// properties/multiproperties datatype, reading from input file
//
// Copyright (c) 2014 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2014 by The University of Warwick
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Standard Library
#include <iostream>
#include <fstream>
#include <string>

// Third Party
// - A

// This Project
#include "PropertyParser.hpp"

int main(int argc, const char *argv[])
{
  std::ifstream input(argv[1]);
  input.unsetf(std::ios::skipws);

  warwick::Property config;

  if (parse_istream(input, config)) {
    std::cout << "Successful parse of \"" << argv[1] << "\"" << std::endl;
    std::cout << "Property = " << config << std::endl;
  } else {
    // NB, even failure may leave us with a partially config object...
    // for example, key may have been set but nothing else.
    std::cerr << "Failed to parse \"" << argv[1] << "\"" << std::endl;
  }

  return 0;
}

