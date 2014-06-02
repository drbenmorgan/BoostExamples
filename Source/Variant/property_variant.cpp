// property_variant - simple tests of variant for LPC
// properties datatype
//
// Basically a hierarchical structure of named variables
// The variable types may be
//  integer
//  real
//  string
//  boolean
//  bitset
//  list of integer
//  list of real
//  list of string
//
// A good example of a hierarchical structure are XML/JSON
// documents. In XML, following the Boost.Spirit guide, we
// can represent the structure with the grammar
//
// xml = start >> *node >> end
// node = xml | data
//
// start/end are the tags, which is sort ot equivalent to
// the name of a property. In data structure terms we have:
//
// struct xml {
//  string name;
//  vector<node> children;
// }
//
// struct node {
//  variant<string,xml> data;
// }
//
// A "property" is, at the most basic level
//
// property = name >> value
//
// Then "properties" is
//
// properties = *property
//
// In this sense, "properties" is like a directory, but without a name.
// Also property is like a file, name as filename, value as content.
// If a properties has a name (and as it has a description, it
// probably should), then we could adapt the xml grammar as
//
// bxml = name >> *bxnode
// bxnode = bxml | value
//
// properties = name >> *property
//
// and
//
// value = data | properties

// Copyright (c) 2014 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2014 by The University of Warwick
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Standard Library
#include <iostream>
#include <iterator>
#include <algorithm>
#include <set>

// Third Party
// - Boost
#include "boost/variant.hpp"
#include "boost/dynamic_bitset.hpp"
#include "boost/utility/enable_if.hpp"
#include "boost/mpl/contains.hpp"

//----------------------------------------------------------------------
// Basic types
//
namespace warwick {
  class Property;
}


namespace warwick {
/// Property object mapping between a name and a value
class Property {
 public:
  /// id of property
  typedef std::string key_type;

  /// types of values the property can take
  typedef boost::variant<int,
          double,
          bool,
          std::string,
          boost::dynamic_bitset<>,
          std::vector<int>,
          std::vector<double>,
          std::vector<std::string>,
          boost::recursive_wrapper<std::vector<warwick::Property> > > value_type;

 public:
  Property();
  explicit Property(const key_type& name);
  ~Property();
  
  bool operator<(const warwick::Property& rhs) const;
  bool operator==(const warwick::Property& rhs) const;
  
  // assignment from any type in value_type, but no others?
  template <typename T>
  typename boost::enable_if<boost::mpl::contains<Property::value_type::types, T>, Property&>::type
  operator=(const T& rhs);

  /// Return copy of value
  template <typename T>
  T getValue() const;

 private:
  key_type Key;
  value_type Value;
};
}


//----------------------------------------------------------------------
// Implementation details
//
/// Visitor for obtaining current type
/// Variant also provides interface, but only returns sequence number
template <typename T>
class is_type_visitor : public boost::static_visitor<bool> {
public:
  template <typename U>
  bool operator()(const U& arg) const {
    return typeid(T) == typeid(U);
  }
};


/// Visitor that outputs a scalar or sequence to supplied ostream
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
    // Copy doesn't work...?
    //std::copy(arg.begin(), arg.end(), std::ostream_iterator<U>(os_,","));
  }
};

//----------------------------------------------------------------------
// Property implementation
//
namespace warwick {
Property::Property() : Key(), Value() {
}

Property::Property(const Property::key_type& name)
  : Key(name), Value() {
}

Property::~Property() {
}

bool Property::operator<(const warwick::Property& rhs) const {
  return Key < rhs.Key;
}
  
bool Property::operator==(const warwick::Property& rhs) const {
  return Key == rhs.Key;
}

template <typename T>
typename boost::enable_if<boost::mpl::contains<Property::value_type::types, T>, Property&>::type
Property::operator=(const T& rhs) {
  Value = rhs;
  return *this;
}

template <typename T>
T Property::getValue() const {
  return boost::get<T>(Value);
}
} // namespace warwick


bool test_collection() {
  std::set<warwick::Property> pset;
  pset.insert(warwick::Property("foo"));
  pset.insert(warwick::Property("bar"));
  
  return true;
}

int main() {
  std::cout << "[boost-examples]" << std::endl;
  std::cout << ">>> testing property objects" <<std::endl;
  
  warwick::Property p("foo");
  p = 1.0;
  
  p = warwick::Property("bar");

  try {
    bool b = p.getValue<bool>();
  } catch (const boost::bad_get& e) {
    std::cerr << "bad get caught" << std::endl;
  }
  
  test_collection();
  return 0;
}
