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

   template<typename T>
   class Mixin
   {
   public:

      typedef T Base;

      // Convert this object to yaml
      std::string to_yaml(void)
      {
         // Create an emitter
         YAML::Emitter emitter;

         // Emit yaml
         return emit(emitter);
      }

      // Convert this object to json
      std::string to_json(void)
      {
         // Create an emitter
         YAML::Emitter emitter;

         // Set the emitter manipulators for json
         emitter << YAML::Flow;
         emitter << YAML::DoubleQuoted;
         emitter << YAML::TrueFalseBool;
         emitter << YAML::EscapeNonAscii;

         // Emit json
         return emit(emitter);
      }

      // Load yaml into this object
      bool from_yaml(std::string const& yaml_string)
      {
         // Create a root node to load into
         ::YAML::Node root;

         try
         {
            // Try loading the root node
            root = YAML::Load(yaml_string);
         }
         catch(...)
         {
            // The yaml couldn't be parsed
            std::cout << "Invalid yaml" << std::endl;
            return false;
         }

         return from_yaml(root);
       }

      // The yaml parser can also parse json
      inline bool from_json(std::string const& json_string)
      {
         return from_yaml(json_string);
      }

      // Load Yaml directly from a Node
      bool from_yaml(YAML::Node& node) {
        // Get a range representing the size of the structure
         typedef typename sequence<Base>::indices indices;

         // Create an extractor for the root node
         extractor<Base> extractor(node);

         // Extract each member of the structure
         try
         {
            // An exception is thrown if any item in the loop cannot be read
            boost::fusion::for_each(boost::fusion::zip(indices(), self()), extractor);
         }
         // Catch all exceptions and prevent them from propagating
         catch(...)
         {
            return false;
         }

         // If we made it here, all fields were read correctly
         return true;
      }


   protected:

      std::string emit(YAML::Emitter& emitter)
      {
         // Get a range representing the size of the structure
         typedef typename sequence<Base>::indices indices;

         // Make a root node to insert into
         ::YAML::Node root;

         // Create an inserter for the root node
         inserter<Base> inserter(root);

         // Insert each member of the structure
         boost::fusion::for_each(boost::fusion::zip(indices(), self()), inserter);

         // Emit yaml
         emitter << root;

         // Return string representation
         return emitter.c_str();
      }

   private:

      // Cast ourselves to our CRTP base
      Base& self(void)
      {
         return static_cast<Base&>(*this);
      }
   };

#endif // YAMLMIXIN_HH

