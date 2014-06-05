// - Property : basic property type
//
// Copyright (c) 2014 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2014 by The University of Warwick
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef PROPERTY_HH
#define PROPERTY_HH
// Standard Library
#include <string>
#include <vector>

// Third Party
// - Boost
#include "boost/variant.hpp"
#include "boost/dynamic_bitset.hpp"

// This Project

namespace warwick {
struct Property {
  typedef std::string key_type;
  typedef boost::variant<int,
                         double,
                         bool,
                         std::string,
                         boost::dynamic_bitset<>,
                         std::vector<int>,
                         std::vector<double>,
                         std::vector<std::string>,
                         boost::recursive_wrapper<std::vector<warwick::Property> > > value_type;
  key_type Key;
  value_type Value;

  /// Visitor that outputs the property value to a given ostream
  struct ostream_visitor : public boost::static_visitor<void> {
  public:
    ostream_visitor(std::ostream& os) : os_(os) {}

    std::ostream& os_;

    template <typename T>
    void operator()(const T& arg) const {
      os_ << arg;
    }

    template<typename U>
    void operator()(const std::vector<U>& arg) const {
      typename std::vector<U>::const_iterator iter = arg.begin();
      typename std::vector<U>::const_iterator end = arg.end();
      while (iter != end) {
        os_ << *iter << ",";
        ++iter;
      }
      // Copy doesn't work...
      //std::copy(arg.begin(), arg.end(), std::ostream_iterator<U>(os_,","));
    }
  };
};

typedef std::vector<warwick::Property> PropertyDocument;

} // namespace warwick

// Output streams for convenience
std::ostream& operator<<(std::ostream& os, const warwick::Property& p) {
  // need a vistor for sequence types
  os << "[" << "key: " << p.Key << "," << "value[" << p.Value.which() << "]: ";
  boost::apply_visitor(warwick::Property::ostream_visitor(os),p.Value);
  os << "]";
  return os;
}

std::ostream& operator<<(std::ostream& os, const warwick::PropertyDocument& d) {
  warwick::PropertyDocument::const_iterator iter = d.begin();
  warwick::PropertyDocument::const_iterator end = d.end();
  while (iter != end) {
    os << *iter << "\n";
    ++iter;
  }
  return os;
}


#endif // PROPERTY_HH
