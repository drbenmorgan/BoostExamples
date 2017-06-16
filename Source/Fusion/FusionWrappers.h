#ifndef FUSIONWRAPPERS_HH
#define FUSIONWRAPPERS_HH

// Standard Library

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
  // Return the demangled string name of the type
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

// Only valid for (a) is_class<S>::value == true, (b) S is a fusion sequence
template<typename S, typename N>
struct element_at {
  // Type of the struct member at this index
  typedef typename boost::fusion::result_of::value_at<S, N>::type type;

  // Previous member
  typedef typename boost::mpl::prior<N>::type previous;

  // Next member
  typedef typename boost::mpl::next<N>::type next;

  // Name of the struct member at this index
  static inline std::string name(void) {
    return boost::fusion::extension::struct_member_name<S, N::value>::call();
  }

  // Demangled name of the type held in this element
  static inline std::string type_name(void) {
    return metadata<type>::name();
  }

  // Access
  static inline typename boost::fusion::result_of::at<S const, N>::type get(S const& s) {
    return boost::fusion::at<N>(s);
  }
};

#endif // FUSIONWRAPPERS_HH

