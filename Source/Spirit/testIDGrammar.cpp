#include "catch.hpp"

// Simple struct for id
#include <boost/fusion/adapted/struct/define_struct.hpp>
#include <boost/fusion/include/define_struct.hpp>

BOOST_FUSION_DEFINE_STRUCT(
    (falaise),
    event_id,
    (int, run)
    (int, event))

std::ostream& operator<<(std::ostream& os, falaise::event_id& id) {
  os << id.run << "_" << id.event;
  return os;
}

#include <iostream>
#include <iterator>
#include <algorithm>
#define BOOST_SPIRIT_DEBUG
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_int.hpp>
#include <boost/spirit/include/qi_char.hpp>

#include <boost/spirit/include/support_istream_iterator.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phx = boost::phoenix;

template <typename Iterator>
struct IDGrammar : public qi::grammar<Iterator,falaise::event_id()> {
  IDGrammar() : IDGrammar::base_type(eid_rule) {
    eid_rule = qi::int_ > '_' > qi::int_;

    BOOST_SPIRIT_DEBUG_NODE(eid_rule);
    //qi::on_error<qi::fail>(eid_rule,
    //                       std::cout << phx::val("Error! Expecting ")
    //                       << qi::labels::_4
    //                       <<std::endl
    //                       );

  }

  qi::rule<Iterator, falaise::event_id()> eid_rule;
};

TEST_CASE("Defined struct works", "[data]") {
  falaise::event_id x {1,2};

  REQUIRE( x.run == 1 );
  REQUIRE( x.event == 2);

  x.run = 4;
  x.event = 7;

  REQUIRE( x.run == 4 );
  REQUIRE( x.event == 7 );
}


TEST_CASE("Basic Grammar", "[grammar]") {
  std::string input("# hello 1_2\n3_6\n 4_5 ");
  std::vector<falaise::event_id> output;

  auto start = input.begin();
  auto end = input.end();
  IDGrammar<decltype(start)> p;

  bool r = qi::phrase_parse(
      start,
      end,
      (p % +qi::eol)
      >> qi::omit[*ascii::space]
      > qi::eoi,
      // Problem? Comment can't be last line [seemingly fixed - can be followed
      // by eol OR eoi] ?
      ascii::blank | qi::lexeme['#' >> *(qi::char_ - qi::eol) >> (qi::eol | qi::eoi)],
      output
      );

  std::cout << "parser result = " << (r ? "true" : "false") << std::endl;
  std::cout << "iterator state = " << ((start == end) ? "true" : "false") << std::endl;
  std::cout << "size = " << output.size() << std::endl;
}



