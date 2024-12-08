#include "CppTemplate/Print.hpp"

#include <catch2/catch_all.hpp>

#include <sstream>

TEST_CASE("CppTemplate") {
  SECTION("Print") {
    std::ostringstream oss;

    mguid::Print(oss);

    REQUIRE(!oss.str().empty());
  }
}