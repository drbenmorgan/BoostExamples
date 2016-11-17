//! Test use of Boost Fusion (later Hana) struct adaption
// Copyright (c) 2016, Ben Morgan <Ben.Morgan@warwick.ac.uk>
//
// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.
//

// Standard Library
#include <iostream>

// Third Party
// - Boost
#include "boost/fusion/sequence.hpp"
#include "boost/fusion/container/map.hpp"
#include "boost/fusion/support/pair.hpp"
#include "boost/fusion/include/adapt_struct.hpp"

#include "boost/mpl/range_c.hpp"
#include "boost/units/detail/utility.hpp"

// This Project
template<typename T>
struct metadata {
  // Return the string name of the type
  static inline std::string name(void) {
    return boost::units::detail::demangle(typeid(T).name());
  }
};


template<typename S>
struct sequence {
  // Point to the first element
  typedef boost::mpl::int_<0> begin;

  // Point to the element after the last element in the sequence
  typedef typename boost::fusion::result_of::size<S>::type end;

  // Point to the first element
  typedef boost::mpl::int_<0> first;

  // Point to the second element (for pairs)
  typedef boost::mpl::int_<1> second;

  // Point to the last element in the sequence
  typedef typename boost::mpl::prior<end>::type last;

  // Number of elements in the sequence
  typedef typename boost::fusion::result_of::size<S>::type size;

  // Get a range representing the size of the structure
  typedef boost::mpl::range_c<unsigned int, 0, boost::mpl::size<S>::value> indices;
};

template<typename S, typename N>
struct element_at {
  // Type of the element at this index
  typedef typename boost::fusion::result_of::value_at<S, N>::type type;

  // Previous element
  typedef typename boost::mpl::prior<N>::type previous;

  // Next element
  typedef typename boost::mpl::next<N>::type next;

  // Member name of the element at this index
  static inline std::string name(void) {
    return boost::fusion::extension::struct_member_name<S, N::value>::call();
  }

  // Type name of the element at this index
  static inline std::string type_name(void) {
    //return boost::units::detail::demangle(typeid(type).name());
    return metadata<type>::name();
  }

  // Access the element
  static inline typename boost::fusion::result_of::at<S const, N>::type get(S const& s) {
    return boost::fusion::at<N>(s);
  }
};


//----------------------------------------------------------------------
// Build up iteration over a fusion sequence using recusrsion
template <typename Sequence, typename Index>
struct struct_visitor_recursive {
  // Where are we in the sequence?
  typedef typename element_at<Sequence, Index>::type current_t;
  typedef typename element_at<Sequence, Index>::next next_t;

  static inline void visit(Sequence const& s) {
    std::cout << element_at<Sequence, Index>::name()
              << " : "
              << element_at<Sequence, Index>::type_name()
              << " = "
    // This is where we would add stuff for recursing into the element
    // if it's a struct
              << element_at<Sequence, Index>::get(s)
              << "\n";

    // Move to next element in sequence
    struct_visitor_recursive<Sequence, next_t>::visit(s);
  }
};

// Stop specialization
template <typename Sequence>
struct struct_visitor_recursive<Sequence, typename sequence<Sequence>::end> {
  static inline void visit(Sequence const& /*s*/) {
  }
};

// Initialize
template <typename Sequence>
struct struct_visitor_initiate : struct_visitor_recursive<Sequence, typename sequence<Sequence>::begin> {
};


//----------------------------------------------------------------------
// Fusion map style type-to-string mapping
// Now mostly handled by the demangling, plus specialization
//
struct TypeTraits {
  typedef boost::mpl::vector<int,double,bool,std::string> types;
  // Transform this to a fusion map of T->std::string
  typedef boost::fusion::map<
      boost::fusion::pair<int, std::string>,
      boost::fusion::pair<double, std::string>,
      boost::fusion::pair<bool, std::string>,
      boost::fusion::pair<std::string,std::string> > type_map;
  static const type_map as_string;

  template <typename T>
  static std::string nameOf() {
    return boost::fusion::at_key<T>(TypeTraits::as_string);
  }
};

const TypeTraits::type_map TypeTraits::as_string = TypeTraits::type_map {
      boost::fusion::make_pair<int>("integer"),
      boost::fusion::make_pair<double>("real"),
      boost::fusion::make_pair<bool>("boolean"),
      boost::fusion::make_pair<std::string>("string")};


//----------------------------------------------------------------------
//! The flat struct
struct Config {
  int foo = 42;
  double bar = 3.14;
  bool baz = true;
  std::string bob = "hello world";
};

BOOST_FUSION_ADAPT_STRUCT(Config,
                          foo,
                          bar,
                          baz,
                          bob)

// Essentially need serialize/deserialize checks,
// Config makeFrom(Properties p) {
//   Config c;
//   for member in Config:
//     c.member = p.get<type Config.member>(member);
//
//   return c;
// }
// Properties makeFrom(Config c) {
//   Properties p;
//   for member in c:
//     p[member] = c.member
//
//    return p
// }
// NB, these are *not* parsers, it's pure setting/schema validation
// from a nested key-value structure. Above ignore error checking/nesting...

int main(int /*argc*/, const char*[]) {
  // What can we do with the adapted sequence?

  // Print members?
  Config c;
  struct_visitor_initiate<Config> s;
  s.visit(c);

  return 0;
}



