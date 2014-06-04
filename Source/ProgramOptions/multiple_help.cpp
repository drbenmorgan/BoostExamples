// multiple_help - study of multiple help combined with required
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
#include <boost/version.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

// This Project
void handle_help(bool val) {
  std::cout << "++ help handled : " << val << std::endl;
}

void handle_help_foo(const std::string& val) {
  std::cout << "++ help-foo handled for foo : " << val << std::endl;
}

void handle_required(int val) {
  std::cout << "++ required handled : " << val << std::endl;
}

int main(int argc, char** argv) {
  namespace po = boost::program_options;

  po::options_description desc("Allowed options");
  desc.add_options()
      ("help,h", po::bool_switch()->notifier(&handle_help),"Print help message")
      ("help-foos", "Print information on available foos")
      ("help-foo",
       po::value<std::string>()
// Only on Boost 1.50 and higher
#if BOOST_VERSION>=105000
       ->value_name("[foo]")
#endif
       ->notifier(&handle_help_foo),
       "Print help on a specific foo")
      ("required",
       po::value<int>()
       ->required()
       ->notifier(&handle_required),
       "Must be present");


  po::parsed_options known = po::command_line_parser(argc, argv).
      options(desc).run();
  po::variables_map vm;

  std::cout << "-- About to store..." << std::endl;
  po::store(known, vm);

  // Before notify
  std::cout << "-- before notify:" << std::endl;
  BOOST_FOREACH(po::variables_map::value_type v, vm) {
    std::cout << v.first << std::endl;
  }

  std::cout << "-- About to notify..." << std::endl;
  po::notify(vm);

  // After notify
  std::cout << "-- after notify:" << std::endl;
  BOOST_FOREACH(po::variables_map::value_type v, vm) {
    std::cout << v.first << std::endl;
  }

  return 0;
}
