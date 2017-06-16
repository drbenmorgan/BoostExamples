#ifndef YAMLNODEOPERATIONS_HH
#define YAMLNODEOPERATIONS_HH
// Standard Library
#include <typeinfo>
// Third Party
// - YAML-CPP
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/exceptions.h"

// This Project
#include "FusionWrappers.h"

//! Insert element in node
template<typename T>
struct inserter {
  typedef T Type;

  inserter(YAML::Node& subroot) : mSubroot(subroot)
  {}

  template<typename Zip>
  void operator()(Zip const& zip) const {
    typedef typename boost::remove_const<
              typename boost::remove_reference<
                typename boost::fusion::result_of::at_c<Zip, 0>::type
               >::type
            >::type Index;

    // Get the field name as a string using reflection
    std::string field_name = element_at<Type, Index>::name();

    // Get the field type - note that this is a reference per
    // behaviour of at_c and decltype
    typedef decltype(boost::fusion::at_c<1>(zip)) FieldType;

    // Alias the member
    FieldType const& member = boost::fusion::at_c<1>(zip);

    // Store this field in the yaml node
    // This will recursively call the YAML::convert<FieldType>::encode...
    mSubroot[field_name] = member;
  }

 protected:
  YAML::Node& mSubroot;
};


//! Extract element from node
template<typename T>
struct extractor {
  typedef T Type;

  extractor(YAML::Node& subroot) :
      mSubroot(subroot),
      mItem(0) {
  }

  template<typename Zip>
  void operator()(Zip const& zip) const {
    typedef typename std::remove_const<
              typename std::remove_reference<
                typename boost::fusion::result_of::at_c<Zip, 0>::type
              >::type
            >::type Index;

    // Get the field native type, remembering that:
    // 1) at_c returns a ref-qualified type
    // 2) the zip is const, so return is also const
    typedef typename std::remove_const<
              typename std::remove_reference<
                decltype(boost::fusion::at_c<1>(zip))
              >::type
            >::type FieldType;

    // Get the field name as a string using reflection
    std::string field_name = element_at<Type, Index>::name();

    // Alias the member
    // We need to const cast this because "boost::fusion::for_each"
    // requires input zip to be const, however we want to modify the elements
    FieldType& member = const_cast<FieldType&>(boost::fusion::at_c<1>(zip));

    // We need to const cast this because "boost::fusion::for_each"
    // requires that operator() be const, however we want to modify
    // the object. This item number is used for error reporting.
    int const& const_item = mItem;
    int& item = const_cast<int&>(const_item);

    // Try to load the value from the file
    try {
      // Extract this field from the yaml node
      member = mSubroot[field_name].template as<FieldType>();

      // This item number helps us find issues when loading incomplete yaml files
      ++item;
    }
    // Catch YAML exceptions
    catch(YAML::Exception const& e) {
      // Print out some helpful information to find the error in the yaml file
      std::string type_name = metadata<FieldType>::name();
      std::cout << "Error loading item " << item  << " : " << type_name << " " << field_name << std::endl;

      // "boost::fusion::for_each" requires operator() to return void,
      // so the only way to signal that an error occurred is to throw
      // an exception.
      throw;
    }
  }

protected:
   YAML::Node& mSubroot; //< YAML node representing Type to be converted
   int mItem;            //< Index of current element
};

#endif // YAMLNODEOPERATIONS_HH

