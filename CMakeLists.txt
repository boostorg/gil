#
# Copyright (c) 2017 Mateusz Loskot <mateusz at loskot dot net>
# All rights reserved.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required(VERSION 3.5.1)
project(Boost.GIL CXX)

include(CMakeDependentOption)
option(GIL_BUILD_TESTS "Build GIL tests" ON)
option(GIL_BUILD_EXAMPLES "Build GIL examples" OFF) # FIXME: Switch to ON after https://github.com/boostorg/gil/issues/40
option(GIL_ENABLE_IO "Enable GIL IO in tests and examples (require libjpeg, libpng, libtiff)" ON)
option(GIL_USE_BOOST_STAGE "Use Boost from current source tree and libraries from stage, unless BOOST_ROOT specified." ON)
option(GIL_USE_CONAN "Use Conan to install dependencies" OFF)

#-----------------------------------------------------------------------------
# Dependency: Boost
# - look for stage Build
# - look for default installation location
# - look for location specified with BOOST_ROOT
#-----------------------------------------------------------------------------
if(GIL_USE_BOOST_STAGE AND NOT DEFINED BOOST_ROOT AND NOT DEFINED ENV{BOOST_ROOT})
  get_filename_component(_boost_root ../../ ABSOLUTE)
  if(EXISTS ${_boost_root}/boost-build.jam)
    set(BOOST_ROOT ${_boost_root})
    message(STATUS "Using Boost libraries from stage directory in BOOST_ROOT=${BOOST_ROOT}")
  endif()
endif()

set(BOOST_COMPONENTS)
if(GIL_BUILD_TESTS)
  list(APPEND BOOST_COMPONENTS unit_test_framework filesystem)
endif()

set(Boost_DETAILED_FAILURE_MSG ON)
if(MSVC)
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_USE_STATIC_RUNTIME OFF)
endif()

find_package(Boost 1.66.0 REQUIRED COMPONENTS ${BOOST_COMPONENTS})

# The boostorg/gil repository includes must come first,
# before Boost includes from cloned Boost superproject or installed distribution.
# Otherwise the IDE sees the wrong file (ie. due to boost/ symlinks or
# GIL headers from installed Boost instead of this clone of boostog/gil).
include_directories(include)

#-----------------------------------------------------------------------------
# Dependency: libpng, libjpeg, libtiff via Vcpkg or Conan
#-----------------------------------------------------------------------------
if (GIL_ENABLE_IO)
  if(GIL_USE_CONAN)
    # Download automatically, you can also just copy the conan.cmake file
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
      message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
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
  else()
    find_package(JPEG)
    find_package(PNG)
    find_package(TIFF)
  endif()
endif()

#-----------------------------------------------------------------------------
# Compiler
#-----------------------------------------------------------------------------
if(MSVC)
  string(REGEX REPLACE "/W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  string(REGEX REPLACE "-W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  add_compile_options(-W4 -FC) # We need absolute path in __FILE__
else()
  add_compile_options(-Wall)
  add_compile_options(-pedantic)
endif()

if(MSVC)
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE=1)
  add_definitions(-D_SCL_SECURE_NO_DEPRECATE=1)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS=1)
  add_definitions(-DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE=1)
endif()

#-----------------------------------------------------------------------------
# Tests
#-----------------------------------------------------------------------------
if(GIL_BUILD_TESTS)
  message(STATUS "Configuring Boost.GIL tests")
  enable_testing()
  add_subdirectory(test)
  if (GIL_ENABLE_IO)
    add_subdirectory(io/test)
  endif()
  add_subdirectory(numeric/test)
  add_subdirectory(toolbox/test)
endif()

#-----------------------------------------------------------------------------
# Examples
#-----------------------------------------------------------------------------
if(GIL_BUILD_EXAMPLES AND GIL_ENABLE_IO)
  add_subdirectory(example)
endif()
