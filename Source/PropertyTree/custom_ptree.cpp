// custom_ptree - test use of boost::variant as data type in ptree
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
#include "boost/foreach.hpp"

// This Project
#include "PropertyTreeTools.hpp"

// Use a blank type in the first entry so that the variant can model
// "unset" behaviour
typedef boost::variant<boost::blank,
                       int,
                       double,
                       bool,
                       std::string> property_value;

// Simple ostream visitor for pretty-printing variant
struct ostream_visitor : public boost::static_visitor<void> {
 public:
  ostream_visitor(std::ostream& os) : os_(os) {}
  std::ostream& os_;

  template <typename T>
  void operator()(const T& arg) const {
    os_ << arg;
  }

  void operator()(const boost::blank& arg) const {}

  //template<typename U>
  //void operator()(const std::vector<U>& arg) const {
  //  typename std::vector<U>::const_iterator iter = arg.begin();
  //  typename std::vector<U>::const_iterator end = arg.end();
  //  while (iter != end) {
  //    os_ << *iter << ",";
  //    ++iter;
  //  }
  //}
};

std::ostream& operator<<(std::ostream& os, const property_value& p) {
  boost::apply_visitor(ostream_visitor(os), p);
  return os;
}


// need a translator between internal and external type
template <typename Internal, typename External>
struct property_value_translator {
  // Needed typedefs
  typedef Internal internal_type;
  typedef External external_type;

  boost::optional<External> get_value(const Internal& value) const {
    return boost::get<External>(value);
  }

  boost::optional<Internal> put_value(const External& value) const {
    return Internal(value);
  }
};

// If we want to extract the underlying data type, have to do a self
// translation seemingly...
struct self_translation {
  // Needed typedefs
  typedef property_value internal_type;
  typedef property_value external_type;

  boost::optional<property_value> get_value(const property_value& value) const {
    return value;
  }

  boost::optional<property_value> put_value(const property_value& value) const {
    return property_value(value);
  }
};



// Actual translator must live in boost::property_tree namespace
namespace boost {
namespace property_tree {
template <typename External>
struct translator_between<property_value, External> {
  typedef property_value_translator<property_value, External> type;
};

template <>
struct translator_between<property_value, property_value> {
  typedef self_translation type;
};
}
}

typedef boost::property_tree::basic_ptree<std::string, property_value> properties;

void display(const properties& t, int depth=0) {
  BOOST_FOREACH(properties::value_type const& v, t.get_child("")) {
    // Process current node
    property_value nodeStr = t.get<properties::data_type>(v.first);

    std::cout << std::string("").assign(depth*2, ' ') << "* ";
    std::cout << v.first;
    std::cout<< " : " << t.get<properties::data_type>(v.first);
    std::cout << std::endl;
    // Handle subtree
    properties subtree = v.second;
    display(subtree, depth+1);
  }
}

int main(int argc, const char *argv[])
{
  properties p;
  p.put("alpha", 1);
  p.put("bravo", 3.14);
  p.put("charlie", true);
  p.put("delta", std::string("hello"));

  properties q;
  q.put("verbosity", "info");

  p.put_child("logging", q);

  display(p);
  return 0;
}
