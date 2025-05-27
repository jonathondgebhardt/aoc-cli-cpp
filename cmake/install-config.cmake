set(aoc-cli_FOUND YES)

include(CMakeFindDependencyMacro)
find_dependency(fmt)

if(aoc-cli_FOUND)
  include("${CMAKE_CURRENT_LIST_DIR}/aoc-cliTargets.cmake")
endif()
