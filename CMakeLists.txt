cmake_minimum_required(VERSION 3.16)

project(BLUSH
    VERSION 0.1.0
    DESCRIPTION "FACE Technical Standard 3.2 aligned C++ libraries"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Build type" FORCE)
endif()

option(BLUSH_BUILD_TESTS "Build BLUSH tests" ON)

include(GNUInstallDirs)
include(CTest)

if(BLUSH_BUILD_TESTS AND BUILD_TESTING)
    include(FetchContent)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.15.2
    )
    FetchContent_MakeAvailable(googletest)
endif()

# ---------------------------------------------------------------------------
# FACE-IDL-Parser tooling (https://github.com/curtcampbell/FACE-IDL-Parser)
#
# UoP code that plugs into FACE::Core / FACE::Util (Injectable interfaces,
# TypedTS connections, uop-connections.xml wiring) is generated with the
# face-idl-gen / face-codegen CLI tools from that project, driven by the
# templates in uop-generator/. blush_add_uop_libraries() (cmake/BLUSHUopCodegen.cmake)
# wraps that pipeline; uop-generator's own CMakeLists.txt uses it as a
# worked example, and downstream consumers get it via find_package(BLUSH).
# ---------------------------------------------------------------------------
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BLUSHUopCodegen.cmake")

add_subdirectory(face-core)
add_subdirectory(face-utils)
add_subdirectory(uop-generator)

# ---------------------------------------------------------------------------
# Package export so downstream projects can find_package(BLUSH)
# ---------------------------------------------------------------------------
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/BLUSHConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/BLUSHConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/BLUSHConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/BLUSH"
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/BLUSHConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BLUSHConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/BLUSH"
)

install(
    EXPORT BLUSHTargets
    FILE BLUSHTargets.cmake
    NAMESPACE FACE::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/BLUSH"
)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/BLUSHUopCodegen.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/BLUSH"
)

install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/uop-generator/templates/"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/blush/uop-templates"
)
