#
# Copyright (c) 2017 Mateusz Loskot <mateusz at loskot dot net>
# All rights reserved.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
message(STATUS "Boost.GIL: configuring Numeric extension tests")

foreach(name
  numeric)

  add_executable(test_ext_${name} "")
  target_sources(test_ext_${name} PRIVATE ${name}.cpp)
  target_link_libraries(test_ext_${name}
    PRIVATE
      gil_compile_options
      gil_include_directories
      Boost::disable_autolinking
      Boost::unit_test_framework)
  add_test(test.ext.${name} test_ext_${name})
endforeach()
