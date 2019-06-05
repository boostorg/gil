# Boost.GIL (Generic Image Library) - tests
#
# Copyright 2019 Miral Shah <miralshah2211@gmail.com>
#
# Use, modification and distribution are subject to the Boost Software License,
# Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
foreach(_name
        binary
        truncate)
    set(_target test_threshold_${_name})

    add_executable(${_target} "")
    target_sources(${_target} PRIVATE ${_name}.cpp)
    target_link_libraries(${_target}
            PRIVATE
            gil_compile_options
            gil_include_directories
            gil_dependencies)
    target_compile_definitions(${_target} PRIVATE BOOST_GIL_USE_CONCEPT_CHECK)
    add_test(test.threshold.${_name} ${_target})

    unset(_name)
    unset(_target)
endforeach()
