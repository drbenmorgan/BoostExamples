#ifndef YAMLSTRUCTCONVERSION_HH
#define YAMLSTRUCTCONVERSION_HH
// Standard Library

// Third Party
// - yaml-cpp
#include "yaml-cpp/node/node.h"

// - Boost
#include "boost/utility/enable_if.hpp"
#include "boost/fusion/support/is_sequence.hpp"

// This Project
#include "FusionWrappers.h"
#include "YAMLNodeOperations.h"

namespace YAML
{
   template<typename T>
   struct convert
   {
      // This function will only be available if the template parameter is a boost fusion sequence
      static Node encode(T const& rhs,
                         typename boost::enable_if<typename boost::fusion::traits::is_sequence<T>::type>::type* = 0)
      {
        // For each item in T
        // Call inserter recursively
        // Every sequence is made up of primitives at some level

        // Get a range representing the size of the structure
        typedef typename sequence<T>::indices indices;

        // Make a root node to insert into
        YAML::Node root;

        // Create an inserter for the root node
        inserter<T> inserter(root);

        // Insert each member of the structure
        boost::fusion::for_each(boost::fusion::zip(indices(), rhs), inserter);

        return root;
      }

      // This function will only be available if the template parameter is a boost fusion sequence
      static bool decode(Node const& node, T& rhs,
                         typename boost::enable_if<typename boost::fusion::traits::is_sequence<T>::type>::type* = 0)
      {
        // For each item in T
        // Call extractor recursively
        // Every sequence is made up of primitives at some level

        // Get a range representing the size of the structure
        typedef typename sequence<T>::indices indices;

        // Create an extractor for the root node
        // Yaml-cpp requires node to be const&, but the extractor makes
        // non-const calls to it.
        Node& writable_node = const_cast<Node&>(node);
        extractor<T> extractor(writable_node);

        // Extract each member of the structure
        try
        {
          // An exception is thrown if any item in the loop cannot be read
          boost::fusion::for_each(boost::fusion::zip(indices(), rhs), extractor);
        }
        // Catch all exceptions and prevent them from propagating
        catch(...)
        {
          return false;
        }

        // If we made it here, all fields were read correctly
        return true;
      }
   };
}

#endif // YAMLSTRUCTCONVERSION_HH

