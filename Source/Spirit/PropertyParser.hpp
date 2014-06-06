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
#include "Property.hpp"

/// Parse input string using property grammar, returning true on success
bool parse_string(const std::string& input, warwick::Property& output);

/// Parse input istream using property grammar, returning true on success
bool parse_istream(std::istream& input, warwick::Property& output);

/// Parse input istream using document grammar, returning true on success
bool parse_document(std::istream& input, warwick::PropertyList& output);

#endif // PROPERTYPARSER_HH

