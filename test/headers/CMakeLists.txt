#
# Copyright (c) 2018 Mateusz Loskot <mateusz at loskot dot net>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

# Mimics self-contained header tests defined in Jamfile as file-less targets
# CMake creates .cpp files on disk.
# Advantage is those files can be analysed with clang-tidy, etc.

file(GLOB_RECURSE _headers
  RELATIVE "${CMAKE_SOURCE_DIR}/include/boost/gil"
  "${CMAKE_SOURCE_DIR}/include/boost/gil/*.hpp")

file(READ ${CMAKE_CURRENT_LIST_DIR}/main.cpp _main_content)

add_custom_target(test_headers)

foreach(_header ${_headers})
  string(REPLACE ".hpp" "" _name ${_header})
  string(REPLACE "/" "-" _name ${_name})
  set(_cpp ${CMAKE_BINARY_DIR}/test/headers/${_name}.cpp)

  string(REPLACE "BOOST_GIL_TEST_HEADER" "${_header}" _content "${_main_content}")
  file(WRITE ${_cpp} "${_content}")

  add_library(test_header_${_name} OBJECT)

  target_sources(test_header_${_name}
    PRIVATE
      ${_cpp}
      ${CMAKE_SOURCE_DIR}/include/boost/gil/${_header})

  target_link_libraries(test_header_${_name}
    PRIVATE
      gil_compile_options
      gil_include_directories
      Boost::boost)

  add_dependencies(test_headers test_header_${_name})

  unset(_cpp)
  unset(_content)
  unset(_name)
endforeach()

unset(_header)
unset(_headers)
