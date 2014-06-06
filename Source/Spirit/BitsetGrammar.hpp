// BitsetGrammar - qi grammar for parsing bitsets
//
// Exposes a boost::dynamic_bitset attribute, composed from the grammar
// (for unlimited size bitsets - need to see if PEG can represent
// "N to M" repitition)
//
// Bitset   <- Bits / Hex
// Bits     <- [0-1]+
// Hex      <- "0x" HexDigit+
// HexDigit <- [0-9a-fA-F]

// Copyright (c) 2014 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2014 by The University of Warwick
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BITSETGRAMMAR_HH
#define BITSETGRAMMAR_HH

// Standard Library

// Third Party
// - Boost
#include <boost/dynamic_bitset.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

// This Project

namespace BoostExamples {
namespace bsqi = boost::spirit::qi;
namespace bsascii = boost::spirit::ascii;
namespace bsphx = boost::phoenix;

/// Convert hex string, e.g. 0xF to supplied type
/// It is designed to be used with boost lexical cast
template <typename T>
struct HexTo {
  T value;
  operator T() const {
    return value;
  }

  friend std::istream& operator>>(std::istream& in, HexTo& out) {
    in >> std::hex >> out.value;
    return in;
  }
};

/// Bitset Grammar of up to 64 bits
/// Does not skip because whitespace is significant
template <typename Iterator>
class BitsetParser : public bsqi::grammar<Iterator, boost::dynamic_bitset<>()> {
 public:
  BitsetParser() : BitsetParser::base_type(bitset) {
    // bitsets can be synthesized as a sequence of 0s and 1s
    // or as hexadecimal. We parse as strings and then use
    // phoenix conversion specialized for hex/bit notation.
    // This split is used so that attributes have correct
    // compatibility:
    // http://boost.2283326.n4.nabble.com/Spirit-Qi-variant-with-std-string-td2715777.html
    // NB: I'm not a spirit expert, so likely this can be done better!
    bitset = bits[bsqi::_val = bsphx::construct<boost::dynamic_bitset<> >(bsqi::_1)]
             |
             hex[bsqi::_val = HexConverter_(bsqi::_1)];

    bits %= bsqi::repeat(1,64)[bsqi::char_("01")];
    hex  %= bsqi::string("0x") > bsqi::repeat(1,16)[bsascii::xdigit];
  }

 private:
  /// Convert a string representing a bitset in hexadecimal form to
  /// a boost dynamic_bitset. It's done in this odd way because
  /// we use a conversion via unsigned long, and we have to
  /// construct the bitset with number of bits *and* the value.
  struct HexConverterImpl {
    template <typename T1>
    struct result {typedef boost::dynamic_bitset<> type;};

    boost::dynamic_bitset<> operator()(const std::string& s) const
    {
      size_t nxdigits(s.size() - 2);
      unsigned long value(boost::lexical_cast<BoostExamples::HexTo<unsigned long> >(s));
      return boost::dynamic_bitset<>(nxdigits*4,value);
    }
  };

 private:
  typedef bsqi::rule<Iterator,boost::dynamic_bitset<>()> bitset_rule_t;
  typedef bsqi::rule<Iterator,std::string()> bitset_string_t;
  bitset_string_t bits;
  bitset_string_t hex;
  bitset_rule_t bitset;
  bsphx::function<HexConverterImpl> HexConverter_;
};
} // namespace BoostExamples

#endif // BITSETGRAMMAR_HH


