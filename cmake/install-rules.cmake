if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/aoc-cli-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
# should match the name of variable set in the install-config.cmake script
set(package aoc-cli)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT aoc-cli_Development
)

install(
    TARGETS aoc-cli_aoc-cli
    EXPORT aoc-cliTargets
    RUNTIME #
    COMPONENT aoc-cli_Runtime
    LIBRARY #
    COMPONENT aoc-cli_Runtime
    NAMELINK_COMPONENT aoc-cli_Development
    ARCHIVE #
    COMPONENT aoc-cli_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    aoc-cli_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE aoc-cli_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(aoc-cli_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${aoc-cli_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT aoc-cli_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${aoc-cli_INSTALL_CMAKEDIR}"
    COMPONENT aoc-cli_Development
)

install(
    EXPORT aoc-cliTargets
    NAMESPACE aoc-cli::
    DESTINATION "${aoc-cli_INSTALL_CMAKEDIR}"
    COMPONENT aoc-cli_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
