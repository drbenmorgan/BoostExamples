// basic_variant - trivial test of a boost::variant
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com> 
// Copyright (c) 2013 by The University of Warwick
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Standard Library
#include <iostream>

// Third Party
// - Boost
#include <boost/variant.hpp>

// This Project

int main(int, char*[]) {
  typedef boost::variant<int, double, std::string> bvtype;

  bvtype a;
  a = 1;
  std::cout << "(value, type) = " << a << ", " << a.which() << std::endl;
  
  a = 1.;
  std::cout << "(value, type) = " << a << ", " << a.which() << std::endl;
 
  a = "foobar";
  std::cout << "(value, type) = " << a << ", " << a.which() << std::endl;

  // Implicit conversion...
  a = true;
  std::cout << "(value, type) = " << a << ", " << a.which() << std::endl;

  return 0;
}

