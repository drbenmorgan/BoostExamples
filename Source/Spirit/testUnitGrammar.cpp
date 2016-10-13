#include "catch.hpp"
#include "UnitsGrammar.hpp"

using namespace BoostExamples;

TEST_CASE("Check basic parsing") {
  double dummy {0.0};

  std::string basicValidity {"kg m s"};
  REQUIRE(get_unit_factor(basicValidity.begin(), basicValidity.end(), dummy));

  std::string basicExponents {"kg^-2"};
  REQUIRE(get_unit_factor(basicExponents.begin(), basicExponents.end(), dummy));

  std::string wsIsSignificant {"mm m"};
  REQUIRE_FALSE(get_unit_factor(wsIsSignificant.begin(), wsIsSignificant.end(), dummy));

  std::string chompShouldWork {"  m m m     "};
  REQUIRE(get_unit_factor(chompShouldWork.begin(), chompShouldWork.end(), dummy));
}

TEST_CASE("Check value synthesis") {
  double value {-1.0};

  SECTION("minimal symbol synthesis") {
    std::string kilogram {"kg"};
    get_unit_factor(kilogram.begin(), kilogram.end(), value);
    REQUIRE(value == Approx(1.0));
  }

  SECTION("basic symbol synthesis") {
    std::string meter {"m"};
    get_unit_factor(meter.begin(), meter.end(), value);
    REQUIRE(value == Approx(2.0));
  }

  SECTION("symbol synthesis with positive powers") {
    std::string unit {"m^2"};
    get_unit_factor(unit.begin(), unit.end(), value);
    REQUIRE(value == Approx(4.0));
  }

  SECTION("symbol synthesis with negative powers") {
    std::string unit {"m^-2"};
    get_unit_factor(unit.begin(), unit.end(), value);
    REQUIRE(value == Approx(0.25));
  }

  SECTION("multisymbol synthesis") {
    std::string unit {"kg m s^-2"};
    get_unit_factor(unit.begin(), unit.end(), value);
    REQUIRE(value == Approx(1.0*2.0*std::pow(3.0,-2)));
  }

}
