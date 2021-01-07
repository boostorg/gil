# CMake generated Testfile for 
# Source directory: /home/prathamesh/gil/test/legacy
# Build directory: /home/prathamesh/gil/test/legacy
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(t_legacy_channel "/home/prathamesh/gil/test/legacy/test_legacy_channel")
set_tests_properties(t_legacy_channel PROPERTIES  _BACKTRACE_TRIPLES "/home/prathamesh/gil/test/legacy/CMakeLists.txt;33;add_test;/home/prathamesh/gil/test/legacy/CMakeLists.txt;0;")
add_test(t_legacy_pixel "/home/prathamesh/gil/test/legacy/test_legacy_pixel")
set_tests_properties(t_legacy_pixel PROPERTIES  _BACKTRACE_TRIPLES "/home/prathamesh/gil/test/legacy/CMakeLists.txt;33;add_test;/home/prathamesh/gil/test/legacy/CMakeLists.txt;0;")
add_test(t_legacy_pixel_iterator "/home/prathamesh/gil/test/legacy/test_legacy_pixel_iterator")
set_tests_properties(t_legacy_pixel_iterator PROPERTIES  _BACKTRACE_TRIPLES "/home/prathamesh/gil/test/legacy/CMakeLists.txt;33;add_test;/home/prathamesh/gil/test/legacy/CMakeLists.txt;0;")
add_test(t_legacy_image "/home/prathamesh/gil/test/legacy/test_legacy_image" "/home/prathamesh/gil/test/legacy/gil_reference_checksums.txt")
set_tests_properties(t_legacy_image PROPERTIES  _BACKTRACE_TRIPLES "/home/prathamesh/gil/test/legacy/CMakeLists.txt;58;add_test;/home/prathamesh/gil/test/legacy/CMakeLists.txt;0;")
