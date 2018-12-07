#
# Copyright (c) 2018 Mateusz Loskot <mateusz at loskot dot net>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
list(APPEND _boost_gil_test_headers ${CMAKE_CURRENT_SOURCE_DIR}/channel_test_fixture.hpp)

foreach(name
  channel_concepts
  channel_test_fixture
  channel_traits
  scoped_channel_value
  algorithm_channel_arithmetic
  algorithm_channel_convert
  algorithm_channel_invert
  algorithm_channel_multiply
  algorithm_channel_relation)

  add_executable(test_core_${name} "")
  target_sources(test_core_${name} PRIVATE ${name}.cpp)
  target_link_libraries(test_core_${name}
    PRIVATE
      gil_compile_options
      gil_include_directories
      Boost::disable_autolinking
      Boost::unit_test_framework)
  add_test(test.core.${name} test_core_${name})
endforeach()
