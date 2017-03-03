//  Copyright (c) 2001-2010 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  This example implements a simple utility allowing to print the attribute
//  type as it is exposed by an arbitrary Qi parser expression. Just insert
//  your expression below, compile and run this example to see what Qi is
//  seeing!

#if !defined (DISPLAY_ATTRIBUTE_OF_PARSER_JAN_2010_30_0722PM)
#define DISPLAY_ATTRIBUTE_OF_PARSER_JAN_2010_30_0722PM

#include <iostream>
#include <boost/spirit/include/qi.hpp>

namespace tools
{
    namespace spirit = boost::spirit;

    template <typename Expr, typename Iterator = spirit::unused_type>
    struct attribute_of_parser
    {
        typedef typename spirit::result_of::compile<
            spirit::qi::domain, Expr
        >::type parser_expression_type;

        typedef typename spirit::traits::attribute_of<
            parser_expression_type, spirit::unused_type, Iterator
        >::type type;
    };

    template <typename T>
    void display_attribute_of_parser(T const &)
    {
        // Report invalid expression error as early as possible.
        // If you got an error_invalid_expression error message here,
        // then the expression (expr) is not a valid spirit qi expression.
        BOOST_SPIRIT_ASSERT_MATCH(spirit::qi::domain, T);

        typedef typename attribute_of_parser<T>::type type;
        std::cout << typeid(type).name() << std::endl;
    }

    template <typename T>
    void display_attribute_of_parser(std::ostream& os, T const &)
    {
        // Report invalid expression error as early as possible.
        // If you got an error_invalid_expression error message here,
        // then the expression (expr) is not a valid spirit qi expression.
        BOOST_SPIRIT_ASSERT_MATCH(spirit::qi::domain, T);

        typedef typename attribute_of_parser<T>::type type;
        os << typeid(type).name() << std::endl;
    }
}

#endif





#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include <utility>

namespace bsqi = boost::spirit::qi;

struct lengths_ : bsqi::symbols<char,std::string> {
  lengths_() {
    add
        ("m", "m")
        ("mm", "mm")
        ("cm", "cm")
        ;
  }
} lengths;

struct masses_ : bsqi::symbols<char,std::string> {
  masses_() {
    add
        ("g", "g")
        ("kg", "kg")
        ;
  }
} masses;

struct times_ : bsqi::symbols<char, std::string> {
  times_() {
    add
        ("s", "s")
        ("ms", "ms")
        ;
  }
} times;

//template <typename Symbol, int Exponent>
//struct Quantity {
  //bsqi::rule<Interator, ...> parser
  //parser = Symbol > '^' > Exponent
//}

template <typename Iterator>
bool get_unit_factor(Iterator first, Iterator last) {
  bsqi::rule<Iterator, std::string()> symbol;
  bsqi::rule<Iterator, int()> exponent;
  bsqi::rule<Iterator, std::pair<std::string,int>()> unit;
  bsqi::rule<Iterator, std::vector<std::pair<std::string,int> >()> quantity;

  // Can replace this with symbol table, e.g
  // symbol = lengths | masses | times
  // Can either used as simply validity check (attribute is still string) or to
  // extract factor (attribute is double).
  // May also want to split this up so we can define parsers for specific
  // quantities (e.g. just lengths, or more complicated things like flow rate -
  // basic interface would be list of symbols with associated powers)
  //
  // - pseudo code...
  // speed = lengths > times(-1);
  // acceleration = lengths > times(-2);
  // momentum = masses > speed;
  // force = forces | (masses > acceleration);
  //
  symbol = lengths | masses | times; //+bsqi::char_("a-z");
  // Exponent defaults to 1 if none is parsed.
  exponent = (bsqi::lit('^') > bsqi::int_) | bsqi::attr(1);
  unit = symbol >> exponent;
  quantity = bsqi::lexeme[unit % bsqi::space];

  std::vector<std::pair<std::string,int> > attr;

  bool result = bsqi::phrase_parse(first, last,
    quantity,
    bsqi::space,
    attr
    );

  if(first != last) {
    return false;
  }

  for(const auto& q : attr) {
    std::cout << q.first << ", " << q.second << std::endl;
  }

  return result;
}

bool test_parse(std::string val) {
  std::cout << "[starting test on '" << val << "']:\n";
  bool r = get_unit_factor(val.begin(), val.end());
  if(r) {
    std::cout << "[ok...]\n";
  }
  else {
    std::cout << "[failed...]\n";
  }
  return r;
}


int main(int, char **)
{
  test_parse("m ");
  test_parse("kg^-2");
  test_parse("kg m s^-1");
  test_parse("kgm s^-1");
  test_parse("kg m^-4 s");
  test_parse("N m");
  test_parse("mm m");
  return 0;
}
