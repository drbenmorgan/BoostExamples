// property-parser - simple frontends to parse property input
//
// Copyright (c) 2014 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2014 by The University of Warwick
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef PROPERTYPARSER_HH
#define PROPERTYPARSER_HH

// Standard Library
#include <string>

// Third Party
// - A

// This Project

/// Parse input string using properties grammar, returning true on success
bool parse_string(const std::string& input);

#endif // PROPERTYPARSER_HH

