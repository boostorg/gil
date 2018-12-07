#
# Copyright (c) 2017 Mateusz Loskot <mateusz at loskot dot net>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
message(STATUS "Boost.GIL: configuring core tests")

foreach(name
  channel
  pixel
  pixel_iterator
  promote_integral)

  add_executable(test_core_${name} "")
  target_sources(test_core_${name} PRIVATE ${name}.cpp error_if.cpp)
  target_link_libraries(test_core_${name}
    PRIVATE
      gil_compile_options
      gil_include_directories
      Boost::disable_autolinking
      Boost::filesystem
      Boost::unit_test_framework)
  add_test(test.core.${name} test_core_${name})
endforeach()

add_executable(test_core_image "")
target_sources(test_core_image
  PRIVATE
    error_if.cpp
    image.cpp
    sample_image.cpp)
target_link_libraries(test_core_image
  PRIVATE
    gil_compile_options
    gil_include_directories
    Boost::disable_autolinking
    Boost::filesystem
    Boost::unit_test_framework)
add_test(test.core.image
  test_core_image
  ${CMAKE_CURRENT_SOURCE_DIR}/gil_reference_checksums.txt)

add_subdirectory(algorithm)
add_subdirectory(channel)
add_subdirectory(image_view)
add_subdirectory(point)
