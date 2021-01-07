# CMake generated Testfile for 
# Source directory: /home/prathamesh/gil/test/core
# Build directory: /home/prathamesh/gil/test/core
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(t_utility_promote_integral "/home/prathamesh/gil/test/core/test_utility_promote_integral")
set_tests_properties(t_utility_promote_integral PROPERTIES  _BACKTRACE_TRIPLES "/home/prathamesh/gil/test/core/CMakeLists.txt;24;add_test;/home/prathamesh/gil/test/core/CMakeLists.txt;0;")
add_test(t_utility_test_fixture "/home/prathamesh/gil/test/core/test_utility_test_fixture")
set_tests_properties(t_utility_test_fixture PROPERTIES  _BACKTRACE_TRIPLES "/home/prathamesh/gil/test/core/CMakeLists.txt;24;add_test;/home/prathamesh/gil/test/core/CMakeLists.txt;0;")
subdirs("point")
subdirs("channel")
subdirs("color")
subdirs("color_base")
subdirs("pixel")
subdirs("iterator")
subdirs("locator")
subdirs("image")
subdirs("image_view")
subdirs("algorithm")
subdirs("image_processing")
