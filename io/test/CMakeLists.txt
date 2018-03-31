#
# Copyright (c) 2017 Mateusz Loskot <mateusz at loskot dot net>
# All rights reserved.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
message(STATUS "Configuring Boost.GIL IO tests")

add_definitions(-DBOOST_GIL_IO_TEST_ALLOW_READING_IMAGES=1)
add_definitions(-DBOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES=1)

add_executable(gil_test_ext_io_simple all_formats_test.cpp)
# Unfortunately, ALIAS of imported target is not supported
# see https://github.com/conan-io/conan/issues/2125
if (GIL_USE_CONAN)
  target_link_libraries(gil_test_ext_io_simple
    PRIVATE
    Boost::disable_autolinking
    Boost::filesystem
    Boost::unit_test_framework
    CONAN_PKG::libjpeg
    CONAN_PKG::libpng
    CONAN_PKG::libtiff)
else()
  target_link_libraries(gil_test_ext_io_simple
    PRIVATE
    Boost::disable_autolinking
    Boost::filesystem
    Boost::unit_test_framework
    PNG::PNG
    TIFF::TIFF
    ${JPEG_LIBRARIES})
  target_include_directories(gil_test_ext_io_simple PRIVATE ${JPEG_INCLUDE_DIR})
endif()
add_test(tests.ext.io.simple gil_test_ext_io_simple)
