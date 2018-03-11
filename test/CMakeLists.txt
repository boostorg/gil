#
# Copyright (c) 2017 Mateusz Loskot <mateusz at loskot dot net>
# All rights reserved.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
foreach(name channel pixel pixel_iterator)
  add_executable(gil_test_core_${name} ${name}.cpp error_if.cpp)
  target_link_libraries(gil_test_core_${name}
    PRIVATE
    Boost::disable_autolinking
    Boost::filesystem
    Boost::unit_test_framework)
  add_test(gil.tests.core.${name} gil_test_core_${name})
endforeach()

add_executable(gil_test_core_image image.cpp sample_image.cpp error_if.cpp)
target_link_libraries(gil_test_core_image
    PRIVATE
    Boost::disable_autolinking
    Boost::filesystem
    Boost::unit_test_framework)
add_test(gil.tests.core.image gil_test_core_image ${CMAKE_CURRENT_SOURCE_DIR}/gil_reference_checksums.txt)
