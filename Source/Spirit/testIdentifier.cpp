#include "catch.hpp"
#include "Identifier.hpp"

using namespace BoostExamples;

TEST_CASE("Construct identifiers","") {
  REQUIRE_NOTHROW( Identifier a("foo") );

  REQUIRE_THROWS_AS( Identifier b {"_foo"}, Identifier::InvalidIdentifier );
}
