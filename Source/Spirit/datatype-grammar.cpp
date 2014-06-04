// datatype-grammar - simple tests of spirit::qi grammar for LPC
// properties/multiproperties datatype
//
// Copyright (c) 2014 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2014 by The University of Warwick
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Standard Library
#include <iostream>
#include <string>

// Third Party
// - A

// This Project
#include "PropertyParser.hpp"

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
