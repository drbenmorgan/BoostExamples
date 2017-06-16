//! Test use of Boost Fusion (later Hana) struct/adt adaption
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

// This Project
#include "FusionWrappers.h"

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
              << "element_at<Sequence, Index>::get(s)"
              << "\n";

    // Move to next element in sequence
    struct_visitor_recursive<Sequence, next_t>::visit(s);
  }
};

// Specialization to stop recursion once we reach the end of Sequence
template <typename Sequence>
struct struct_visitor_recursive<Sequence, typename sequence<Sequence>::end> {
  static inline void visit(Sequence const& /*s*/) {
  }
};

// Initialize
template <typename Sequence>
struct make_struct_visitor : struct_visitor_recursive<Sequence, typename sequence<Sequence>::begin> {
};


// To implement generic recursion, the basic pattern is
// - A terminal operation is when we don't have a fusion sequence
//   anymore (going from our YAML example), e.g.
//
// template <typename T>
// void do_something_with(T const& rhs)
//
// This is then specialized on fusion sequences and "anything else"
// See also http://stackoverflow.com/questions/26380420/boost-fusion-sequence-type-and-name-identification-for-structs-and-class
// for an example of a generic visitor.
// We'd like to make "Structure visitation" generic if possible, so that
// different persistency schemes can be handled, e.g. JSON/YAML/Ini/PTree
// with different couplings to the underlying library.
// Have an example here of YAML, which should be similar to NLohmann Json.
// 



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


int main(int /*argc*/, const char*[]) {
  // What can we do with the adapted sequence?

  // Print members?
  Module c;
  make_struct_visitor<Module> s;
  s.visit(c);

  // Currently FusionWrappers assumes adapted struct?
  // Models a random access sequence
  // Yep - assumes that the Type has boost::fusion::extension::struct_member_name
  // template defined.
  //typedef boost::fusion::vector<int, double, std::string> ConfigVector;
  //ConfigVector fv {1, 3.14, "hello world"};
  //make_struct_visitor<ConfigVector> cvs;
  //cvs.visit(fv);
  //
  // We'd like to do that for ADTs as well, the fusion adaptions
  // for them do not associate a name with each getter/setter
  // Can be done with custom macros, but that's probably too
  // awkward

  // Can we simply iterate over the sequence?

  return 0;
}



