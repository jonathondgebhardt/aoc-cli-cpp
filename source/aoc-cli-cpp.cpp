#include <string>

#include "aoc-cli-cpp/aoc-cli-cpp.hpp"

#include <fmt/core.h>

exported_class::exported_class()
    : m_name {fmt::format("{}", "aoc-cli-cpp")}
{
}

auto exported_class::name() const -> char const*
{
  return m_name.c_str();
}
