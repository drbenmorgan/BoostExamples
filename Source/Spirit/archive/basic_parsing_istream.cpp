// basic_parsing - simple tests of spirit::qi parsing using istream input
//                 NB, can of course just dump into string and parse that!
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com> 
// Copyright (c) 2013 by The University of Warwick
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Standard Library
#include <string>
#include <fstream>

// Third Party
// - Boost
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

// This Project

/// parse input string as comma-separated list of doubles
void parse_istream(std::istream& input) {
  using namespace boost::spirit;

  istream_iterator first(input);
  istream_iterator last;

  bool result = qi::phrase_parse(
      first,
      last,
      qi::double_ >> *(',' >> qi::double_),
      qi::space
      );

  if (!result) {
    std::cout << "Failed to parse istream" << std::endl;
    return;
  }

  if (first != last) {
    std::cout << "No complete parse of istream" << std::endl;
    return;
  }

  std::cout << "Successful parse of istream" << std::endl;
}


int main(int argc, const char *argv[])
{
  std::ifstream in(argv[1]);
  in.unsetf(std::ios::skipws);
  parse_istream(in);
  std::cout << "[done]\n";

  return 0;
}

