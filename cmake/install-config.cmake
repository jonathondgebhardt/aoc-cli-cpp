set(aoc-cli-cpp_FOUND YES)

include(CMakeFindDependencyMacro)
find_dependency(fmt)

if(aoc-cli-cpp_FOUND)
  include("${CMAKE_CURRENT_LIST_DIR}/aoc-cli-cppTargets.cmake")
endif()
