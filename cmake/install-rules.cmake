if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/aoc-cli-cpp-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
# should match the name of variable set in the install-config.cmake script
set(package aoc-cli-cpp)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT aoc-cli-cpp_Development
)

install(
    TARGETS aoc-cli-cpp_aoc-cli-cpp
    EXPORT aoc-cli-cppTargets
    RUNTIME #
    COMPONENT aoc-cli-cpp_Runtime
    LIBRARY #
    COMPONENT aoc-cli-cpp_Runtime
    NAMELINK_COMPONENT aoc-cli-cpp_Development
    ARCHIVE #
    COMPONENT aoc-cli-cpp_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    aoc-cli-cpp_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE aoc-cli-cpp_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(aoc-cli-cpp_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${aoc-cli-cpp_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT aoc-cli-cpp_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${aoc-cli-cpp_INSTALL_CMAKEDIR}"
    COMPONENT aoc-cli-cpp_Development
)

install(
    EXPORT aoc-cli-cppTargets
    NAMESPACE aoc-cli-cpp::
    DESTINATION "${aoc-cli-cpp_INSTALL_CMAKEDIR}"
    COMPONENT aoc-cli-cpp_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
