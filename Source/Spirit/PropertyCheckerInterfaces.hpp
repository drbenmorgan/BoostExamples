// - PropertyCheckerInterfaces.hpp - basic ifaces to property parsing
// Copyright (c) 2014 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2014 by The University of Warwick
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef PROPERTYCHECKERINTERFACES_HH
#define PROPERTYCHECKERINTERFACES_HH

// - read and validate Property format text file
int filereader_main(const char* filename);

// - run command line interface for Property interpreter
int cli_main();

#endif // PROPERTYCHECKERINTERFACES_HH

