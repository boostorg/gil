# CMake generated Testfile for 
# Source directory: /home/prathamesh/gil/test
# Build directory: /home/prathamesh/gil/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(t_utility_test_utility_output_stream "/home/prathamesh/gil/test/test_utility_test_utility_output_stream")
set_tests_properties(t_utility_test_utility_output_stream PROPERTIES  _BACKTRACE_TRIPLES "/home/prathamesh/gil/test/CMakeLists.txt;22;add_test;/home/prathamesh/gil/test/CMakeLists.txt;0;")
subdirs("core")
subdirs("legacy")
subdirs("extension")
subdirs("header")
