#ifndef YAMLMIXIN_HH
#define YAMLMIXIN_HH
// Standard Library
#include <iostream>

// Third Party
// - yaml-cpp
#include "yaml-cpp/emitter.h"
#include "yaml-cpp/node/parse.h"
#include "yaml-cpp/node/node.h"

// - Boost
#include <boost/fusion/algorithm/transformation/zip.hpp>

// This Project
#include "FusionWrappers.h"
#include "YAMLStructConversion.h"

// Try to yamlize without inheritance
// Presumes Fusion-ization of any type converted
class YAMLizer {
 public:
  //! Only enable to/from yaml/json if type is a Fusion sequence
  template <typename T>
  using IsFusionSequence = typename boost::enable_if<typename boost::fusion::traits::is_sequence<T>::type, int>::type;

 public:
  // Convert this object to yaml IAOI T is a Fusion Sequence
  template <typename T, IsFusionSequence<T> = 0>
  static std::string to_yaml(const T& rhs)
  {
    // Create an emitter
    YAML::Emitter emitter;

    // Emit yaml
    return YAMLizer::emit(emitter, rhs);
  }

  // Convert this object to json
  template <typename T, IsFusionSequence<T> = 0>
  static std::string to_json(const T& rhs)
  {
    // Create an emitter
    YAML::Emitter emitter;

    // Set the emitter manipulators for json
    emitter << YAML::Flow;
    emitter << YAML::DoubleQuoted;
    emitter << YAML::TrueFalseBool;
    emitter << YAML::EscapeNonAscii;

    // Emit json
    return YAMLizer::emit(emitter, rhs);
  }

  // Construct object from Yaml
  template <typename T>
  static T from_yaml(std::string const &yaml_string)
  {
    // Create a root node to load into
    ::YAML::Node root;

    try
    {
      // Try loading the root node
      root = YAML::Load(yaml_string);
    }
    catch (...)
    {
      // The yaml couldn't be parsed
      std::cout << "Invalid yaml" << std::endl;
      throw;
    }

    return YAMLizer::from_yaml<T>(root);
  }

  // The yaml parser can also parse json
  template <typename T>
  static T from_json(std::string const &json_string)
  {
    return YAMLizer::from_yaml<T>(json_string);
  }

  // Load Yaml directly from a Node
  template <typename T>
  static T from_yaml(YAML::Node &node)
  {
    // Get a range representing the size of the structure
    typedef typename sequence<T>::indices indices;

    // Create an extractor for the root node
    extractor<T> extractor(node);

    // Create new instance
    T output;

    // Extract each member of the structure
    try
    {
      // An exception is thrown if any item in the loop cannot be read
      boost::fusion::for_each(boost::fusion::zip(indices(), output), extractor);
    }
    // Catch all exceptions and rethrow
    catch (...)
    {
      throw;
    }

    // If we made it here, all fields were read correctly, return instance
    return output;
  }

protected:
  template <typename T>
  static std::string emit(YAML::Emitter &emitter, const T& rhs)
  {
    // Get a range representing the size of the structure
    typedef typename sequence<T>::indices indices;

    // Make a root node to insert into
    ::YAML::Node root;

    // Create an inserter for the root node
    inserter<T> inserter(root);

    // Insert each member of the structure
    boost::fusion::for_each(boost::fusion::zip(indices(), rhs), inserter);

    // Emit yaml
    emitter << root;

    // Return string representation
    return emitter.c_str();
  }
};

#endif // YAMLMIXIN_HH

