#
# Copyright (c) 2018 Mateusz Loskot <mateusz at loskot dot net>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
foreach(name
  collection
  image_view_concepts)

  add_executable(test_core_${name} "")
  target_sources(test_core_${name} PRIVATE ${name}.cpp)
  target_link_libraries(test_core_${name}
    PRIVATE
      gil_compile_options
      gil_include_directories
      Boost::boost )
  add_test(test.core.${name} test_core_${name})
endforeach()
