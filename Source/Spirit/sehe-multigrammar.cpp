// - This code taken from Sehe's post of stackoverflow:
// http://stackoverflow.com/questions/9678440/is-it-possible-to-re-use-boostspiritqi-grammar-in-another-grammar-definition

#include <string>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

struct Date {
    int year, month, day;
};
struct Time {
    int hour, minute, second;
};

BOOST_FUSION_ADAPT_STRUCT(
    Date,
    (int, year)
    (int, month)
    (int, day)
)

BOOST_FUSION_ADAPT_STRUCT(
    Time,
    (int, hour)
    (int, minute)
    (int, second)
)

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
typedef std::string::const_iterator Iterator;

class DateParser: public qi::grammar < Iterator, Date() > {
    public:
        DateParser(): base_type(main) {
            main %= qi::int_ >> '-' >> // Year
                    qi::int_ >> '-' >> // Month
                    qi::int_;          // Day
        }
    private:
        qi::rule < Iterator, Date() > main;
};

class TimeParser : public qi::grammar < Iterator, Time() > {
    public:
        TimeParser(): base_type(main) {
            main %= qi::int_ >> ':' >> // Hour
                    qi::int_ >> ':' >> // Minute
                    qi::int_;          // Second
        }
    private:
        qi::rule < Iterator, Time() > main;
};

class DateTimeParser:
    public qi::grammar < Iterator, boost::variant<Date, Time>() > {
        qi::rule < Iterator, boost::variant<Date, Time>()> main;
    public:
        DateTimeParser(): base_type(main) {
            main %= date_parser | time_parser;
        }
        DateParser date_parser;
        TimeParser time_parser;
};

#include<iostream>
#include<cstdio>

struct Printer : public boost::static_visitor<> {
    void operator()(Date a) const {
        printf("Year: %d, Month: %d, Day: %d\n", a.year, a.month, a.day);
    }
    void operator()(Time a) const {
        printf("Hour: %d, Minute: %d, Second: %d\n", a.hour, a.minute, a.second);
    }
};

int main() {
    std::string s;
    std::getline(std::cin, s);
    Iterator beg = s.begin(), end = s.end();
    boost::variant<Date, Time> ret;
    phrase_parse(beg, end, DateTimeParser(), ascii::space, ret);
    if (beg != end)
        puts("Parse failed.");
    else
        boost::apply_visitor(Printer(), ret);
}
