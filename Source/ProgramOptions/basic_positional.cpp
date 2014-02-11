// basic_positional - trivial test of positional arguments
//
// Copyright (c) 2014 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2014 by The University of Warwick
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Standard Library
#include <iostream>

// Third Party
// - Boost
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

// This Project

int main(int argc, char** argv) {
  namespace po = boost::program_options;

  po::options_description desc("Allowed Options");
  desc.add_options()
      ("help,h", "produce help message")
      ("tool", po::value<std::string>(), "tool to run");

  po::parsed_options known = po::command_line_parser(argc, argv).
      options(desc).
      allow_unregistered().
      run();

  // Print known options
  std::cout << "Known options received: " << std::endl;
  po::variables_map vm;
  po::store(known, vm);
  po::notify(vm);

  BOOST_FOREACH(po::variables_map::value_type v, vm) {
    std::cout << v.first << std::endl;
  }


  std::cout << "Unrecognized options for passing onwards:" << std::endl;
  std::vector<std::string> pass = po::collect_unrecognized(known.options, po::include_positional);
  BOOST_FOREACH(std::string e, pass) {
    std::cout << e << std::endl;
  }

  return 0;
}

