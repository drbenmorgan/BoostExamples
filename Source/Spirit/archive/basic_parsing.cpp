// basic_parsing - simple tests of spirit::qi parsing
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com> 
// Copyright (c) 2013 by The University of Warwick
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Standard Library
#include <string>

// Third Party
// - Boost
#include <boost/spirit/include/qi.hpp>

// This Project

/// parse input string as comma-separated list of doubles
void parse_string(const std::string& input) {
  using namespace boost::spirit;

  std::string::const_iterator first(input.begin());
  std::string::const_iterator last(input.end());

  bool result = qi::phrase_parse(
      first,
      last,
      qi::double_ >> *(',' >> qi::double_),
      qi::space
      );

  if (!result) {
    std::cout << "Failed to parse \"" << input << "\"" << std::endl;
    return;
  }

  if (first != last) {
    std::cout << "No complete parse of \"" << input << "\"" << std::endl;
    return;
  }

  std::cout << "Successful parse of \"" << input << "\"" << std::endl;
}


int main(int argc, const char *argv[])
{
  std::cout << "[basic_parsing] qi parsing of comma separated list\n";
  std::cout << "Enter a comma separated list of numbers at the prompt\n";
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

