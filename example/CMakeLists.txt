#
# Copyright (c) 2017 Mateusz Loskot <mateusz at loskot dot net>
# All rights reserved.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
message(STATUS "Configuring Boost.GIL examples tests")

# MWE is dedicated for GIL users and contributors, compiled source file is ignored by git
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/minimal_working_example.cpp)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/minimal_working_example.cpp.in
    ${CMAKE_CURRENT_SOURCE_DIR}/minimal_working_example.cpp
    COPYONLY)
endif()

file(GLOB _examples ${CMAKE_CURRENT_LIST_DIR}/*.cpp CONFIGURE_DEPEND)

foreach(example ${_examples})
  get_filename_component(name ${example} NAME_WE)
  add_executable(gil_example_${name} ${name}.cpp)
  target_compile_definitions(gil_example_${name} PRIVATE BOOST_GIL_USE_CONCEPT_CHECK=1)
  # Unfortunately, ALIAS of imported target is not supported
  # see https://github.com/conan-io/conan/issues/2125
  if (GIL_USE_CONAN)
    target_link_libraries(gil_example_${name}
      PRIVATE
      Boost::disable_autolinking
      Boost::filesystem
      CONAN_PKG::libjpeg
      CONAN_PKG::libpng
      CONAN_PKG::libtiff)
  else()
    target_link_libraries(gil_example_${name}
      PRIVATE
      Boost::disable_autolinking
      Boost::filesystem
      PNG::PNG
      TIFF::TIFF
      ${JPEG_LIBRARIES})
    if (JPEG_LIBRARIES AND JPEG_INCLUDE_DIR)
      target_include_directories(gil_example_${name} PRIVATE ${JPEG_INCLUDE_DIR})
    endif()
  endif()
endforeach()

