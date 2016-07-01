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

// This Project
#include "PropertyCheckerInterfaces.hpp"

int main(int /*argc*/, const char *argv[])
{
  int result(0);
  if (argv[1]) {
    result = filereader_main(argv[1]);
  } else {
    result = cli_main();
  }
  return result;
}
