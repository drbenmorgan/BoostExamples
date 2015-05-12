// wildcards - how does PO handle wildcards in args?
//             Example usage: `ls *.txt`
//
// Copyright (c) 2014 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2014 by The University of Warwick
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Standard Library
#include "glob.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>

// Third Party
// - Boost
#include <boost/program_options.hpp>
#include <boost/regex.hpp>


//void autoExpandEnvironmentVariables( std::string & text ) {
//    static boost::regex env( "\\$\\{([^}]+)\\}" );
//    boost::smatch match;
//    while ( boost::regex_search( text, match, env ) ) {
//        const char * s = getenv( match[1].str().c_str() );
//        const std::string var( s == NULL ? "" : s );
//        text.replace( match[0].first, match[0].second, var );
 //   }
//}


std::string matchToEnv(boost::smatch const& match) {
  const char * s = getenv( match[1].str().c_str() );
  return s == NULL ? std::string("") : std::string(s);
}

std::string expandEnvironmentVariables(const std::string& in) {
  static boost::regex re( "\\$\\{([^}]+)\\}" );
  return boost::regex_replace(in, re, matchToEnv);
}





int main(int argc, char **argv) {
  namespace po = boost::program_options;

  typedef std::vector<std::string> filelist_t;
  filelist_t iPuts;

  // - Set up
  po::options_description desc("Allowed Options");
  desc.add_options()
      ("help,h", "produce help message")
      // Multitoken means '-i foo bar baz' works
      ("input-file,i", po::value<filelist_t>(&iPuts)->multitoken(), "input files");

  po::positional_options_description pDesc;
  pDesc.add("input-file", -1);

  // - Parse
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
            options(desc).positional(pDesc).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 0;
  }

  std::cout << "User provided inputs : " << iPuts.size() << std::endl;

  std::cout << "Raw:" << std::endl;
  std::copy(iPuts.begin(),
            iPuts.end(),
            std::ostream_iterator<filelist_t::value_type>(std::cout,"\n"));

  // Handle expansion of regex expressions, envvars?

  // envvars first, because they have different syntax
  std::cout << "ENVVARS Expanded:" << std::endl;
  filelist_t expandEnvVars;
  std::transform(iPuts.begin(), iPuts.end(), std::back_inserter(expandEnvVars),expandEnvironmentVariables);


  // Now actual regex?
  // - If we're on UNIX, glob may be an option
  filelist_t::iterator start = expandEnvVars.begin();
  filelist_t::iterator end = expandEnvVars.end();
  filelist_t globED;

  while (start != end) {
    glob_t globBuffer;
    std::cout << "trying to glob : " << *start << std::endl;
    glob((*start).c_str(), GLOB_TILDE, NULL, &globBuffer);
    std::cout

    for(int i(0); i < globBuffer.gl_pathc; ++i) {
      globED.push_back(globBuffer.gl_pathv[i]);
    }

    globfree(&globBuffer);
    ++start;
  }


  std::copy(globED.begin(),
            globED.end(),
            std::ostream_iterator<filelist_t::value_type>(std::cout,"\n"));





  return 0;
}
