// Identifier - define properties identifier
// Used as a key in properties, so equivalent to a C-style identifer
// Copyright (c) 2014 by Ben Morgan <Ben.Morgan@warwick.ac.uk>
// Copyright (c) 2014 by The University of Warwick
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef IDENTIFIER_HH
#define IDENTIFIER_HH

#include <regex>

namespace BoostExamples {
class Identifier {
 public:
  struct InvalidIdentifier {};
 public:
  explicit Identifier(const std::string& id);
  ~Identifier() = default;

  const std::string& value() const {
    return id_;
  }

 private:
  std::string id_;
  static const std::regex cIdMatcher;
};

class Description {
 public:
  Description(const std::string& brief, const std::string& details) : brief_(brief), details_(details), value_(brief+" "+details) {}
  ~Description() = default;

  const std::string& value() const {
    return value_;
  }

  const std::string& brief() const {
    return brief_;
  }

  const std::string& details() const {
    return details_;
  }

 private:
  std::string brief_;
  std::string details_;
  std::string value_;
};


const std::regex BoostExamples::Identifier::cIdMatcher = std::regex("[a-zA-Z0-9][a-zA-Z0-9_]+");

Identifier::Identifier(const std::string& id) {
  if (std::regex_match(id,cIdMatcher)) {
    id_ = id;
  } else {
    throw Identifier::InvalidIdentifier();
  }
}

} // namespace BoostExample


#endif // IDENTIFIER_HH

