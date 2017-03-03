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
// Other iteration via for_each?
template <typename T>
struct field_printer {
  typedef T Type;

  template <typename Zip>
  void operator()(Zip const& zip) const {
    typedef typename boost::remove_const<
        typename boost::remove_reference<
        typename boost::result_of::at_c<Zip, 0>::type
        >::type
        >::type Index;

    // Get field name as string
    std::string field_name = element_at<Type, Index>::name();

    // Field type name
    std::string field_type = element_at<Type,Index>::type_name();

    // Field value
    auto field_value =





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

//! The nested struct
struct Module {
  std::string algorithm;
  Config setup;
};

BOOST_FUSION_ADAPT_STRUCT(Module, algorithm, setup)

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
// - Mock properties?
//typedef std::map<std::string, boost::variant<int, double, bool, std::vector<int> > Properties;


int main(int /*argc*/, const char*[]) {
  // What can we do with the adapted sequence?

  // Print members?
  Module c;
  struct_visitor_initiate<Module> s;
  s.visit(c);

  return 0;
}



