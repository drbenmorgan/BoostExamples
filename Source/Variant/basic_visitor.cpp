// basic_visitor - experiment with simple visitors
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Standard Library
#include <iostream>
#include <vector>

// Third Party
// - Boost
#include <boost/variant.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/any.hpp>
#include <boost/foreach.hpp>

// This Project
namespace warwick {
/// boolean variant visitor returning true if template type matches
/// type currently held in visitor
template <typename T>
class is_type_visitor : public boost::static_visitor<bool> {
 public:
  template <typename U>
  bool operator()(const U& /*arg*/) const {
    return typeid(T) == typeid(U);
  }
};

/// Visitor that converts a variant to an any
class as_any_visitor : public boost::static_visitor<boost::any> {
 public:
  template <typename T>
  boost::any operator()(const T& arg) const {
    return boost::any(T(arg));
  }
};


/// return true if type T is present in the MPL Sequence U::types
/// Might be better as a struct, but could make usage harder as
/// both types would need to be specified?
template <typename T, typename U>
bool able_to_store(const U&) {
  return boost::mpl::contains<typename U::types, T>::type::value;
}
} // namespace warwick

int main(int /*argc*/, char *[]) {
  typedef boost::variant<int, double, std::string> bvtype;

  bvtype a;
  a = 1;
  // Type check
  assert(boost::apply_visitor(warwick::is_type_visitor<int>(), a));

  // Storage type check
  assert(warwick::able_to_store<std::string>(a));
  assert(!warwick::able_to_store<size_t>(a));

  // Variant to Any conversion
  std::vector<bvtype> bvVector;
  bvVector.push_back(1);
  bvVector.push_back(3.14);
  bvVector.push_back("foobarbaz");

  BOOST_FOREACH(bvtype& e, bvVector) {
    std::cout << "variant : " << e << std::endl;
  }

  std::vector<boost::any> anyVector;
  anyVector.resize(bvVector.size());
  warwick::as_any_visitor convertor;

  std::transform(bvVector.begin(),
                 bvVector.end(),
                 anyVector.begin(),
                 boost::apply_visitor(convertor));

  // With any this is messy, but it should suceed and output the same
  // numbers as above
  std::cout << "any : " << boost::any_cast<int>(anyVector[0]) << std::endl;
  std::cout << "any : " << boost::any_cast<double>(anyVector[1]) << std::endl;
  std::cout << "any : " << boost::any_cast<std::string>(anyVector[2]) << std::endl;

  return 0;
}

