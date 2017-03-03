#ifndef YAMLNODEOPERATIONS_HH
#define YAMLNODEOPERATIONS_HH
// Standard Library

// Third Party
// - YAML-CPP
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/exceptions.h"

// This Project
#include "FusionWrappers.h"

//! Insert element in node
template<typename T>
struct inserter
{
   typedef T Type;

   inserter(YAML::Node& subroot) :
      mSubroot(subroot)
   {}

   template<typename Zip>
   void operator()(Zip const& zip) const
   {
      typedef typename boost::remove_const<
                 typename boost::remove_reference<
                    typename boost::fusion::result_of::at_c<Zip, 0>::type
                 >::type
              >::type Index;

      // Get the field name as a string using reflection
      std::string field_name = element_at<Type, Index>::name();

      // Get the field type
      //typedef BOOST_TYPEOF(boost::fusion::at_c<1>(zip)) FieldType;
      typedef decltype(boost::fusion::at_c<1>(zip)) FieldType;

      // Alias the member
      FieldType const& member = boost::fusion::at_c<1>(zip);

      // Store this field in the yaml node
      mSubroot[field_name] = member;
   }

protected:
   YAML::Node& mSubroot;
};


//! Extract element from node
template<typename T>
struct extractor
{
   typedef T Type;

   extractor(YAML::Node& subroot) :
      mSubroot(subroot),
      mItem(0)
   {

   }

   template<typename Zip>
   void operator()(Zip const& zip) const
   {
      typedef typename boost::remove_const<
                 typename boost::remove_reference<
                    typename boost::fusion::result_of::at_c<Zip, 0>::type
                 >::type
              >::type Index;

      // Get the field name as a string using reflection
      std::string field_name = element_at<Type, Index>::name();

      // Get the field native type
      typedef BOOST_TYPEOF(boost::fusion::at_c<1>(zip)) FieldType;
      //typedef decltype(boost::fusion::at_c<1>(zip)) FieldType;

      // Alias the member
      // We need to const cast this because "boost::fusion::for_each"
      // requires that zip be const, however we want to modify it.
      FieldType const& const_member = boost::fusion::at_c<1>(zip);
      FieldType& member = const_cast<FieldType&>(const_member);

      // We need to const cast this because "boost::fusion::for_each"
      // requires that operator() be const, however we want to modify
      // the object. This item number is used for error reporting.
      int const& const_item = mItem;
      int& item = const_cast<int&>(const_item);

      // Try to load the value from the file
      try
      {
         // Extract this field from the yaml node
         member = mSubroot[field_name].template as<FieldType>();

         // This item number helps us find issues when loading incomplete yaml files
         ++item;
      }
      // Catch any exceptions
      catch(YAML::Exception const& e)
      {
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
   YAML::Node& mSubroot;
   int mItem;
};

#endif // YAMLNODEOPERATIONS_HH

