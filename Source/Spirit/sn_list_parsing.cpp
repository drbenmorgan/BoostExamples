// sn_parsing - basic parsing of "statement > newline" structure

#define BOOST_SPIRIT_DEBUG
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/fusion/include/std_pair.hpp>
namespace qi = boost::spirit::qi;

typedef std::pair<std::string, std::vector<int> > Statement;
typedef std::vector<Statement> Document;

// Statement grammar
// Insist on blank_type because statements should only be agnostic to
// spaces and tabs
template <typename Iterator>
struct StatementGrammar : qi::grammar<Iterator,Statement(),qi::blank_type> {
  StatementGrammar() : StatementGrammar::base_type(statement) {
    identifier %= qi::alpha >> *(qi::alnum | qi::char_('_'));
    statement = identifier > '='
                > '['
                > qi::skip(qi::space)[qi::int_ % ","]
                > ']';
    BOOST_SPIRIT_DEBUG_NODE(identifier);
    BOOST_SPIRIT_DEBUG_NODE(statement);
  }

  qi::rule<Iterator,std::string()> identifier;
  qi::rule<Iterator,Statement(),qi::blank_type> statement;
};

// Document grammar
// Bracket statements so that preceeding/trailing whitespace are chomped
// Finish with eoi to give complete grammar
template <typename Iterator>
struct DocumentGrammar : qi::grammar<Iterator,Document(), qi::blank_type> {
  DocumentGrammar() : DocumentGrammar::base_type(document) {
    document %= qi::omit[*qi::space]
                >> statement % +qi::char_("\n")
                >> qi::omit[*qi::space]
                > qi::eoi;
    BOOST_SPIRIT_DEBUG_NODE(document);
  }

  StatementGrammar<Iterator> statement;
  qi::rule<Iterator,Document(), qi::blank_type> document;
};

int main(int argc, const char *argv[])
{
  std::ifstream input(argv[1]);
  input.unsetf(std::ios::skipws);


  boost::spirit::istream_iterator first(input);
  boost::spirit::istream_iterator last;

  Document doc;
  DocumentGrammar<boost::spirit::istream_iterator> g;

  bool result = qi::phrase_parse(first, last, g, qi::blank, doc);

  if (result) {
    std::cout << "good parse" << std::endl;
    BOOST_FOREACH(const Statement& s, doc) {
      std::cout << "id: " << s.first << ", value: ";
      std::copy(s.second.begin(),
                s.second.end(),
                std::ostream_iterator<int>(std::cout,","));
      std::cout << std::endl;
    }
  } else {
    std::cerr << "bad parse" << std::endl;
  }

  return 0;
}
