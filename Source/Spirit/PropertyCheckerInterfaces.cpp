// - PropertyCheckerInterfaces.cpp - Implementation
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

int filereader_main(const char* filename) {
  std::ifstream input(filename);
  input.unsetf(std::ios::skipws);

  warwick::PropertyList config;

  if (parse_document(input, config)) {
    std::cout << "Successful parse of \"" << filename << "\"" << std::endl;
    std::cout << "Document = " << config << std::endl;
    return 0;
  } else {
    // NB, even failure may leave us with a partially config object...
    // for example, key may have been set but nothing else.
    std::cerr << "Failed to parse \"" << filename << "\"" << std::endl;
    return 1;
  }
}


int cli_main() {
  std::cout << "[datatype-grammar] qi parsing of properties\n";
  std::cout << "Type [q or Q] to quit\n\n";

  std::string input;

  std::cout << ">>> ";

  while (std::getline(std::cin, input)) {
    if (input.empty() || input[0] == 'q' || input[0] == 'Q') break;

    warwick::Property config;

    if (parse_string(input, config)) {
      std::cout << "Successful parse of \"" << input << "\"" << std::endl;
      std::cout << "Property = " << config << std::endl;
    } else {
      // NB, even failure may leave us with a partially config object...
      // for example, key may have been set but nothing else.
      std::cerr << "Failed to parse \"" << input << "\"" << std::endl;
    }

    std::cout << ">>> ";
  }

  std::cout << "[quit]\n";
  return 0;
}

