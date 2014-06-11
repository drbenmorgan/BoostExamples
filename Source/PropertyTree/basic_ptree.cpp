// basic_ptree - simple tests of boost::property_tree usage
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
#include "boost/foreach.hpp"
#include "boost/property_tree/ptree.hpp"

// This Project
#include "PropertyTreeTools.hpp"

int main()
{
  typedef boost::property_tree::ptree PropertyTree;
  PropertyTree PTree;
  // Create node foo, with an empty child tree
  PTree.put_child("foo", PropertyTree());

  // The node can hold its own data as well as children
  PTree.put("foo", "node data");

  // Can add individual nodes
  PTree.put("baz", "a string");

  // And individual children
  PTree.put("foo.bar", true);
  PTree.put("foo.frob", 1.0);


  // Can also create and add subtree
  // as a child of an existing node
  PropertyTree subTree;
  subTree.put("verbosity", "info");
  subTree.put("channel", "terminal");
  PTree.put_child("foo.logging", subTree);

  display(PTree);
  return 0;
}
