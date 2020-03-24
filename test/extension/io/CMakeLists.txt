#
# Copyright (c) 2017 Mateusz Loskot <mateusz at loskot dot net>
# All rights reserved.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
message(STATUS "Boost.GIL: Configuring tests in test/extension/io")

# TODO: Add <format>/CMakeLists.txt configuration for each format ~mloskot

set(_formats
  simple)

if(BOOST_GIL_ENABLE_EXT_IO_RAW)
  list(APPEND _formats raw)
endif()

foreach(_name ${_formats})
  set(_test t_ext_io_${_name})
  set(_target test_ext_io_${_name})

  add_executable(${_target} "")
  target_link_libraries(${_target}
    PRIVATE
      gil_compile_options
      gil_include_directories
      gil_dependencies)
  target_compile_definitions(${_target}
    PRIVATE
      BOOST_GIL_IO_TEST_ALLOW_READING_IMAGES
      BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES)
  add_test(NAME ${_test} COMMAND ${_target})

  unset(_name)
  unset(_target)
  unset(_test)
endforeach()

unset(_formats)

target_sources(test_ext_io_simple
  PRIVATE
    simple_all_formats.cpp)
