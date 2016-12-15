// UnitsGrammar - qi grammar for parsing scientific units expressions
//
// Example expressions
//
// 1.0 m s^-2
// 5 kg m s^-1
//
// So grammar is a limited calculator grammar, allowing only symbolic factors
// (the unit name), a separator to divide
//
// Quantity <- Real Unit (" " Unit)*
// Unit <- Length | Mass | Time
// Length <- "m" | "mm" | "km" | ...
// Mass <- "kg" | "g" | ...
// Time <- "s" | "ms" | "day" | "yr" | ...
// Exponent <- "^" Integer
// Integer <- Digit+
// Digit <- 0..9
//
// Not completely clear on separation between units and dimensions - input
// would be in units, but symbols, whilst representing units, are really
// separated on basis of dimension (e.g. length, mass, time). It really
// depends what the end attribute of the grammar is -
// 1) a raw double representing the factor we need to multiply the quantity
// by to obtain its value in the internal units system (ala all HEP software)
// 2) an actual type - e.g. Boost.Unit quantity
//
// Logically, for input, only require integer exponents (what would kg^-1/2 mean?),
// but see here:
//
// http://stackoverflow.com/questions/11333135/fractional-power-of-units-of-measures-in-f
//
// That should only really affect internal quantities, e.g. derived units
//
// For now, the parser simply
// - Treats a unit as exposing a double attribute, i.e. conversion factor
// - Only allows integer exponents
// - Only checks that the input is sane, not that it meets a dimensional
//   requirement
//
// In last case, can probably build up parsers for specific types using a
// grammar class templated on symbol types and exponents - pseudo:
//
// typedef area_ unit_grammar<length, -2>;
// typedef speed_ unit_grammar<length, 1, time, -1>
//
// May also be possible to use this further, e.g.
//
// typedef acceleration_ unit_grammar<speed_, 1, time, -1>
//
// NB - should "collapse" to "unit_grammar<length_, 1, time, -2>", but that'll
// be more complex.


#ifndef UNITSGRAMMAR_HH
#define UNITSGRAMMAR_HH

#include <boost/spirit/include/qi.hpp>
// Needed to use pairs in qi grammars/as attributes
#include <boost/fusion/include/std_pair.hpp>

#include <numeric>


namespace BoostExamples {
namespace bsqi = boost::spirit::qi;

// Symbol tables for converting length, mass and time
// Could also be constructed on fly by custom function (e.g. use CLHEP
// to get conversion factors)
struct length_ : bsqi::symbols<char,double> {
  length_() {
    add
        ("m", 2.0)
        ;
  }
} lengths;

struct mass_ : bsqi::symbols<char,double> {
  mass_() {
    add
        ("kg", 1.0)
        ;
  }
} masses;

struct time_ : bsqi::symbols<char,double> {
  time_() {
    add
        ("s", 3.0)
        ;
  }
} times;


// Implement as simpele parse (no skipping) for now
template <typename Iterator>
bool get_unit_factor(Iterator first, Iterator last, double& factor) {
  // The object the grammar should synthesize down to
  std::vector<std::pair<double,int> > attr;

  bool r = bsqi::phrase_parse(first, last,
    // Grammar def
    (
        // Need to non-skip inside because ws is the separator between elements
        bsqi::lexeme[
          ( (lengths | masses | times)
           >>
           ((bsqi::lit('^') > bsqi::int_) | bsqi::attr(1)) )
          % bsqi::space
        ]
    ),
    bsqi::space,
    attr
    );

  if(!r || first != last) {
    return false;
  }

  // Post process the attr for now
  // Ideally want grammar to do this for us - a semantic
  // action is probably easiest given the simplicity of the grammar,
  // but could also consider transform_attribute.
  factor = std::accumulate(attr.begin(), attr.end(),
                           1.0,
                           [](double current, auto& in) {
                             return current*std::pow(in.first, in.second);
                           });

  return r;
}

} // namespace BoostExamples
#endif // UNITSGRAMMAR_HH

