#include <string>

#include "aoc-cli-cpp/aoc-cli-cpp.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Name is aoc-cli-cpp", "[library]")
{
  auto const exported = exported_class {};
  REQUIRE(std::string("aoc-cli-cpp") == exported.name());
}
