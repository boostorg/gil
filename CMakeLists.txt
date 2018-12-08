#
# Copyright (c) 2017-2018 Mateusz Loskot <mateusz at loskot dot net>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required(VERSION 3.10)

#-----------------------------------------------------------------------------
# Version
#-----------------------------------------------------------------------------
file(STRINGS
  include/boost/gil/version.hpp
  BOOST_GIL_LIB_VERSION
  REGEX "#define BOOST_GIL_LIB_VERSION ")

if(BOOST_GIL_LIB_VERSION)
  string(REGEX REPLACE ".*\"(.*)\".*" "\\1" BOOST_GIL_LIB_VERSION "${BOOST_GIL_LIB_VERSION}")
  string(REPLACE "_" "." BOOST_GIL_LIB_VERSION "${BOOST_GIL_LIB_VERSION}")
else()
  message(SEND_ERROR "Cannot find version in '${CMAKE_CURRENT_SOURCE_DIR}/include/boost/gil/version.hpp'")
endif()

#-----------------------------------------------------------------------------
# Project
#-----------------------------------------------------------------------------
project(Boost.GIL
  LANGUAGES CXX
  VERSION ${BOOST_GIL_LIB_VERSION}
  DESCRIPTION "Boost.GIL - Generic Image Library | Requires C++11 since Boost 1.68")

message(STATUS "Boost.GIL: Version ${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(GIL_BUILD_IO "Build GIL IO tests and examples (require libjpeg, libpng, libtiff)" ON)
option(GIL_BUILD_EXAMPLES "Build GIL examples" OFF) # FIXME: Switch to ON after https://github.com/boostorg/gil/issues/40
option(GIL_USE_CONAN "Use Conan to install dependencies" OFF)
option(GIL_DOWNLOAD_FINDBOOST "Download FindBoost.cmake from latest CMake release" OFF)

#-----------------------------------------------------------------------------
# Dependency target
#-----------------------------------------------------------------------------
add_library(gil_dependencies INTERFACE)

#-----------------------------------------------------------------------------
# Dependency: Boost
# - look for stage Build
# - look for default installation location
# - look for location specified with BOOST_ROOT
#-----------------------------------------------------------------------------
if(CMAKE_VERSION VERSION_LESS 3.13 AND NOT GIL_DOWNLOAD_FINDBOOST)
  message(STATUS "Boost.GIL: You are using CMake older than 3.13")
  message(STATUS "Boost.GIL: FindBoost.cmake has likely been updated to detect newer or even not yet released Boost")
  message(STATUS "Boost.GIL: Run CMake with -DGIL_DOWNLOAD_FINDBOOST=ON to get latest version of FindBoost.cmake")
  message(STATUS "Boost.GIL: WARNING:")
  message(STATUS "Boost.GIL:    Newer FindBoost.cmake may fail to find your Boost for many reasons.")
  message(STATUS "Boost.GIL:    For example, this may be due to unrecognised toolset eg. latest Visual Studio 2017.")
  message(STATUS "Boost.GIL:    Try run CMake with -DBoost_COMPILER=\"-vc141\" or value for your toolset.")
endif()

if(GIL_DOWNLOAD_FINDBOOST)
  if(NOT EXISTS "${CMAKE_BINARY_DIR}/cmake/FindBoost.cmake")
    message(STATUS "Boost.GIL: Downloading FindBoost.cmake from https://gitlab.kitware.com/cmake/ release branch")
    file(DOWNLOAD
      "https://gitlab.kitware.com/cmake/cmake/raw/release/Modules/FindBoost.cmake"
      "${CMAKE_BINARY_DIR}/cmake/FindBoost.cmake")
  endif()
  list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_BINARY_DIR}/cmake)
endif()

if(NOT DEFINED BOOST_ROOT AND NOT DEFINED ENV{BOOST_ROOT})
  message(STATUS "Boost.GIL: Looking for Boost from current source tree and libraries from stage.")
  message(STATUS "Boost.GIL: Disable stage look-up with passing -DBOOST_ROOT=/path/to/your/boost.")
  get_filename_component(_boost_root ../../ ABSOLUTE)
  if(EXISTS ${_boost_root}/boost-build.jam)
    set(BOOST_ROOT ${_boost_root})
    message(STATUS "Boost.GIL: Using Boost libraries from stage directory in BOOST_ROOT=${BOOST_ROOT}")
  endif()
endif()

set(Boost_DETAILED_FAILURE_MSG ON)
if(MSVC)
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_USE_STATIC_RUNTIME OFF)
endif()

find_package(Boost 1.65.0 REQUIRED
  COMPONENTS
    filesystem
    unit_test_framework)
message(STATUS "Boost.GIL: using Boost_INCLUDE_DIRS=${Boost_INCLUDE_DIRS}")
message(STATUS "Boost.GIL: using Boost_LIBRARY_DIRS=${Boost_LIBRARY_DIRS}")

target_link_libraries(gil_dependencies
  INTERFACE
    Boost::disable_autolinking
    Boost::filesystem
    Boost::unit_test_framework)

#-----------------------------------------------------------------------------
# Dependency: libpng, libjpeg, libtiff via Vcpkg or Conan
#-----------------------------------------------------------------------------
if(GIL_BUILD_IO)
  if(GIL_USE_CONAN)
    # Download automatically, you can also just copy the conan.cmake file
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
      message(STATUS "Boost.GIL: Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
      file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.9/conan.cmake"
        "${CMAKE_BINARY_DIR}/conan.cmake")
    endif()

    # NOTE: See RelWithDebInfo for Release builds, http://docs.conan.io/en/latest/howtos/vs2017_cmake.html
    set(_build_type_saved ${CMAKE_BUILD_TYPE})
    if(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
      set(CMAKE_BUILD_TYPE "Release")
    endif()

    include(${CMAKE_BINARY_DIR}/conan.cmake)
    conan_cmake_run(CONANFILE conanfile.txt BASIC_SETUP CMAKE_TARGETS)

    set(CMAKE_BUILD_TYPE ${_build_type_saved})
    unset(_build_type_saved)

    target_link_libraries(gil_dependencies
      INTERFACE
        CONAN_PKG::libjpeg
        CONAN_PKG::libpng
        CONAN_PKG::libtiff)
  else()
    find_package(JPEG)
    find_package(PNG)
    find_package(TIFF)

    target_link_libraries(gil_dependencies
      INTERFACE
        ${JPEG_LIBRARIES}
        PNG::PNG
        TIFF::TIFF)

    target_include_directories(gil_dependencies
      INTERFACE
        ${JPEG_INCLUDE_DIR})

    if(UNIX)
      # Typically, Linux packages provide C++ stream interface for TIFF
      find_path(_tiffxx_include_dir NAMES tiffio.hxx)
      find_library(_tiffxx_library NAMES tiffxx)

      target_link_libraries(gil_dependencies
        INTERFACE
          ${_tiffxx_library})
      target_include_directories(gil_dependencies
        INTERFACE
          ${_tiffxx_include_dir})
    endif()

  endif()
endif()

#-----------------------------------------------------------------------------
# Common C++ compilation flags
# Follows https://svn.boost.org/trac10/wiki/Guidelines/WarningsGuidelines
#-----------------------------------------------------------------------------
add_library(gil_compile_options INTERFACE)
target_compile_features(gil_compile_options INTERFACE cxx_std_11)

if(MSVC)
  string(REGEX REPLACE "/W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  string(REGEX REPLACE "-W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

target_compile_options(gil_compile_options
  INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:-W4>
    $<$<CXX_COMPILER_ID:MSVC>:-bigobj>
    $<$<CXX_COMPILER_ID:MSVC>:-FC> # Need absolute path for __FILE__ used in tests
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fstrict-aliasing -pedantic
      -Wall -Wconversion -Wextra -Wfloat-equal -Wshadow -Wsign-promo -Wstrict-aliasing -Wunused-parameter
    >)

target_compile_definitions(gil_compile_options
  INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_NONSTDC_NO_DEPRECATE>
    $<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_DEPRECATE>
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
    $<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>
    $<$<CXX_COMPILER_ID:MSVC>:BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE>)

#-----------------------------------------------------------------------------
# Common include directories
#
# The boostorg/gil repository includes must come first,
# before Boost includes from cloned Boost superproject or installed distribution.
# Otherwise IDEs may see the wrong file (ie. due to boost/ symlinks or
# GIL headers from installed Boost instead of this clone of boostog/gil).
#-----------------------------------------------------------------------------
add_library(gil_include_directories INTERFACE)
target_include_directories(gil_include_directories
  BEFORE
  INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/test)

#-----------------------------------------------------------------------------
# Tests
#-----------------------------------------------------------------------------
enable_testing()
add_subdirectory(test)
if(GIL_BUILD_IO)
  add_subdirectory(io/test)
endif()
add_subdirectory(numeric/test)
add_subdirectory(toolbox/test)

#-----------------------------------------------------------------------------
# Examples
#-----------------------------------------------------------------------------
if(GIL_BUILD_EXAMPLES AND GIL_BUILD_IO)
  add_subdirectory(example)
endif()
