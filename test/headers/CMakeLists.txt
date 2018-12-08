#
# Copyright (c) 2018 Mateusz Loskot <mateusz at loskot dot net>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
message(STATUS "Boost.GIL: configuring self-contained header tests for all headers")

# Mimics self-contained header tests defined in Jamfile as file-less targets
# CMake creates .cpp files on disk.
# Advantage is those files can be analysed with clang-tidy, etc.

file(GLOB_RECURSE _headers
  RELATIVE "${CMAKE_SOURCE_DIR}/include/boost/gil"
  "${CMAKE_SOURCE_DIR}/include/boost/gil/*.hpp")

file(READ ${CMAKE_CURRENT_LIST_DIR}/main.cpp _main_content)

# TODO: Which option is most convenient to build from IDEs?
add_custom_target(test_all_headers)
#add_executable(test_all_headers main.cpp)
#add_library(test_all_headers STATIC main.cpp)

foreach(_header ${_headers})
  string(REPLACE ".hpp" "" _name ${_header})
  string(REPLACE "/" "_" _main ${_name})
  string(REPLACE "/" "-" _name ${_name})
  set(_cpp ${CMAKE_BINARY_DIR}/test/headers/${_name}.cpp)

  string(REPLACE "BOOST_GIL_TEST_HEADER" "${_header}" _content "${_main_content}")
  string(REPLACE "main" "${_main}" _content "${_content}")
  unset(_main)
  file(WRITE ${_cpp} "${_content}")
  unset(_content)

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

  add_dependencies(test_all_headers test_header_${_name})

  unset(_cpp)
  unset(_header)
  unset(_name)
endforeach()

unset(_headers)
