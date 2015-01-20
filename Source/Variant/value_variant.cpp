// value_variant - test a type limited and type locked value

// Copyright (c) 2014 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2014 by The University of Warwick
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Standard Library
#include <iostream>
#include <vector>

// Third Party
// - Boost
#include "boost/variant.hpp"
#include "boost/utility/enable_if.hpp"
#include "boost/mpl/contains.hpp"

//----------------------------------------------------------------------
// Value should have specific behaviour, so create objetc for this
// Whilst mostly based on Boost variant, it adds some special
// behaviour
class value_t {
 public:
  template <typename T>
  struct sequence {
    typedef std::vector<T> type;
    typedef T value_type;
    typedef typename type::iterator iterator;
    typedef typename type::const_iterator const_iterator;
  };

  typedef boost::variant<boost::blank,
                         int,
                         double,
                         bool,
                         std::string,
                         sequence<int>::type,
                         sequence<double>::type,
                         sequence<std::string>::type> holder;

 public:
  /// Default constructor
  value_t();

  /// Allow construction from any of the allowed types
  template <typename T>
  value_t(const T& value, typename boost::enable_if<
                            boost::mpl::contains<holder::types, T>
                            >::type* = 0);

  /// Allow construction from string like type
  value_t(const char* value);

  /// Copy constructor
  /// Compiler supplied should be o.k. because we don't hold
  /// pointers, and variant provides suitable constructors

  /// Destructor
  ~value_t();

  /// Get held value only if requested type is in held type sequence
  /// Throws boost::bad_get if held type is not the requested type
  template <typename T>
  typename boost::enable_if<boost::mpl::contains<holder::types, T>, T>::type
  as() const;

  /// Set held value, only if argument is of type the value can hold
  template <typename T>
  typename boost::enable_if<boost::mpl::contains<holder::types, T>, void>::type
  set_value(const T& v);

  /// Return true if this value can hold a value of the supplied type
  template <typename T>
  bool can_hold() const;

  /// Return true if the currently held value is of the supplied type
  template <typename T>
  bool holds_type() const;

  /// Return true if the value is set
  /// variant is "never empty", but we use a variant with a blank type
  /// to mark an "unset" value
  bool is_set() const;

  /// Return true if the value is an iterable sequence of type
  template <typename T>
  bool is_sequence() const;

  /// Return beginning iterator
  template <typename T>
  typename value_t::sequence<T>::iterator begin() {
    return boost::get<typename value_t::sequence<T>::type>(value_).begin();
  }

  template <typename T>
  typename value_t::sequence<T>::iterator end() {
    return boost::get<typename value_t::sequence<T>::type>(value_).end();
  }


 private:
  /// Visitor to determine if held type is scalar or sequence
  /// of a specific type
  template <typename T>
  struct is_sequence_visitor : boost::static_visitor<bool> {
    template <typename U>
    bool operator()(const U& arg) const {
      return false;
    }

    bool operator()(const typename value_t::sequence<T>::type& arg) const {
      return true;
    }
  };

 private:
  /// The actual value
  holder value_;
};

//----------------------------------------------------------------------
// Implementation details
//
value_t::value_t() {}

template <typename T>
value_t::value_t(const T& value, typename boost::enable_if<boost::mpl::contains<holder::types, T> >::type*) : value_(value) {}

value_t::value_t(const char* value) : value_(std::string(value)) {}

value_t::~value_t() {}

template <typename T>
typename boost::enable_if<boost::mpl::contains<value_t::holder::types, T>, T>::type
value_t::as() const {
  return boost::get<T>(value_);
}

template <typename T>
typename boost::enable_if<boost::mpl::contains<value_t::holder::types, T>, void>::type
value_t::set_value(const T& v) {
  value_ = v;
}

template <typename T>
bool value_t::can_hold() const {
  return boost::mpl::contains<holder::types, T>::value;
}

template <typename T>
bool value_t::holds_type() const {
  return typeid(T) == value_.type();
}

bool value_t::is_set() const {
  return value_.which() == 0 ? false : true;
}

template <typename T>
bool value_t::is_sequence() const {
  return boost::apply_visitor(value_t::is_sequence_visitor<T>(), value_);
}


//----------------------------------------------------------------------
// Strict value - like a value, but can only have one type
// can be assigned from a value, but only if types match

template <typename T>
struct NoValidation {
  static void validate(const T& v) {}
};


template <typename T, template<class> class ValidationPolicy = NoValidation>
class strict_value_t {
 public:
  strict_value_t() {}

  template<typename U>
  typename boost::enable_if<boost::is_same<U, T>, strict_value_t>::type&
  operator=(const U& v) {
    T tmp(v);
    ValidationPolicy<T>::validate(value_);
    value_ = tmp;
    return *this;
  }

  strict_value_t& operator=(const value_t& v) {
    T tmp(v.as<T>());
    ValidationPolicy<T>::validate(value_);
    value_ = tmp;
    return *this;
  }

 private:
  T value_;
};



//----------------------------------------------------------------------
// Test case
#define BOOST_TEST_MODULE ValueVariantTest
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(construction) {
  BOOST_TEST_MESSAGE("Testing value_t construction");

  value_t a;
  BOOST_REQUIRE(!a.is_set());

  value_t b(42);
  BOOST_REQUIRE(b.is_set());
  BOOST_REQUIRE(b.holds_type<int>());
  BOOST_REQUIRE(!b.holds_type<double>());

  value_t c("construction from char*");
  BOOST_REQUIRE(c.holds_type<std::string>());
}

BOOST_AUTO_TEST_CASE(assignment) {
  BOOST_TEST_MESSAGE("Testing value_t assignment");

  value_t v;
  v = 24;
  BOOST_REQUIRE(v.holds_type<int>());
  BOOST_REQUIRE_EQUAL(v.as<int>(),24);

  v = true;
  BOOST_REQUIRE(v.holds_type<bool>());
  BOOST_REQUIRE_EQUAL(v.as<bool>(),true);

  v = 3.14;
  BOOST_REQUIRE(v.holds_type<double>());
  BOOST_REQUIRE_CLOSE(v.as<double>(), 3.14, 0.001);

  v = std::string("hello, world");
  BOOST_REQUIRE(v.holds_type<std::string>());
  BOOST_REQUIRE_EQUAL(v.as<std::string>(), "hello, world");

  v = "some const char*";
  BOOST_REQUIRE(v.holds_type<std::string>());
  BOOST_REQUIRE_EQUAL(v.as<std::string>(), "some const char*");

}


BOOST_AUTO_TEST_CASE(conversion) {
  BOOST_TEST_MESSAGE("Testing value_t conversion");

  value_t v(42);
  BOOST_REQUIRE(v.holds_type<int>());
  BOOST_REQUIRE(!v.holds_type<short>());

  typedef int IntSynonym;
  BOOST_REQUIRE(v.holds_type<IntSynonym>());

  BOOST_REQUIRE_THROW(v.as<double>(), boost::bad_get);

  int i = v.as<int>();
  BOOST_REQUIRE_EQUAL(i, 42);
}

BOOST_AUTO_TEST_CASE(scalarvector) {
  BOOST_TEST_MESSAGE("Testing value_t scalar/vector");

  value_t v;
  value_t::sequence<int>::type s;
  s.push_back(5);
  s.push_back(2);
  v = 1;
  BOOST_REQUIRE(!v.is_sequence<int>());
  BOOST_REQUIRE_THROW(v.begin<int>(), boost::bad_get);
  v = s;
  BOOST_REQUIRE(v.is_sequence<int>());
  BOOST_REQUIRE(!v.is_sequence<double>());

  BOOST_REQUIRE_EQUAL((*v.begin<int>()), 5);
}

BOOST_AUTO_TEST_CASE(strict) {
  value_t v = 24;

  strict_value_t<double> s;

  BOOST_REQUIRE_THROW(s = v, boost::bad_get);
  s = 24.0;

}

