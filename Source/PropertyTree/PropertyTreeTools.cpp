// PropertyTreeTools implementation
// Ourselves
#include "PropertyTreeTools.hpp"

// Standard Library
#include <iostream>

// Third Party
// - Boost
#include "boost/foreach.hpp"
#include "boost/property_tree/ptree.hpp"

// This Project

void display(const boost::property_tree::ptree& t, int depth) {
  BOOST_FOREACH(boost::property_tree::ptree::value_type const& v, t.get_child("")) {
    // Process current node
    std::string nodeData = t.get<std::string>(v.first);

    std::cout << std::string("").assign(depth*2, ' ') << "* ";
    std::cout << v.first;
    if (nodeData.length() > 0) {
      std::cout<< "=\"" << nodeData << "\"";
    }
    std::cout << std::endl;
    // Handle subtree
    boost::property_tree::ptree subtree = v.second;
    display(subtree, depth+1);
  }
}

