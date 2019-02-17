#
# Copyright (c) 2017 Mateusz Loskot <mateusz at loskot dot net>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
message(STATUS "Boost.GIL: Configuring core tests")

# All-in-one tests, legacy
# TODO: To be refactored to modular
foreach(_name
  channel
  image
  pixel
  pixel_iterator
  promote_integral)
  set(_target all_in_one_${_name})

  add_executable(${_target} "")
  target_sources(${_target} PRIVATE ${_name}.cpp error_if.cpp)
  target_link_libraries(${_target}
    PRIVATE
      gil_compile_options
      gil_include_directories
      gil_dependencies)
  target_compile_definitions(${_target} PRIVATE BOOST_GIL_USE_CONCEPT_CHECK)
  add_test(test.all_in_one.${_name} ${_target})

  unset(_target)
  unset(_name)
endforeach()

# Add extra source files accompanying image.cpp
target_sources(all_in_one_image
  PRIVATE
    error_if.cpp
    sample_image.cpp)
add_test(test.all_in_one.image
  all_in_one_image
  ${CMAKE_CURRENT_SOURCE_DIR}/gil_reference_checksums.txt)

if(GIL_BUILD_HEADERS_TESTS)
  add_subdirectory(headers)
endif()

# Modular tests
add_subdirectory(point)
add_subdirectory(channel)
add_subdirectory(pixel)
add_subdirectory(image)
add_subdirectory(image_view)
add_subdirectory(algorithm)
