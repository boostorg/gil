# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/prathamesh/gil

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/prathamesh/gil

# Include any dependencies generated for this target.
include test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/depend.make

# Include the progress variables for this target.
include test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/progress.make

# Include the compile flags for this target's objects.
include test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/flags.make

test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/get_num_bits.cpp.o: test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/flags.make
test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/get_num_bits.cpp.o: test/extension/toolbox/get_num_bits.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/prathamesh/gil/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/get_num_bits.cpp.o"
	cd /home/prathamesh/gil/test/extension/toolbox && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_ext_toolbox_get_num_bits.dir/get_num_bits.cpp.o -c /home/prathamesh/gil/test/extension/toolbox/get_num_bits.cpp

test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/get_num_bits.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_ext_toolbox_get_num_bits.dir/get_num_bits.cpp.i"
	cd /home/prathamesh/gil/test/extension/toolbox && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/prathamesh/gil/test/extension/toolbox/get_num_bits.cpp > CMakeFiles/test_ext_toolbox_get_num_bits.dir/get_num_bits.cpp.i

test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/get_num_bits.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_ext_toolbox_get_num_bits.dir/get_num_bits.cpp.s"
	cd /home/prathamesh/gil/test/extension/toolbox && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/prathamesh/gil/test/extension/toolbox/get_num_bits.cpp -o CMakeFiles/test_ext_toolbox_get_num_bits.dir/get_num_bits.cpp.s

# Object files for target test_ext_toolbox_get_num_bits
test_ext_toolbox_get_num_bits_OBJECTS = \
"CMakeFiles/test_ext_toolbox_get_num_bits.dir/get_num_bits.cpp.o"

# External object files for target test_ext_toolbox_get_num_bits
test_ext_toolbox_get_num_bits_EXTERNAL_OBJECTS =

test/extension/toolbox/test_ext_toolbox_get_num_bits: test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/get_num_bits.cpp.o
test/extension/toolbox/test_ext_toolbox_get_num_bits: test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/build.make
test/extension/toolbox/test_ext_toolbox_get_num_bits: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so.1.71.0
test/extension/toolbox/test_ext_toolbox_get_num_bits: /usr/lib/x86_64-linux-gnu/libjpeg.so
test/extension/toolbox/test_ext_toolbox_get_num_bits: /usr/lib/x86_64-linux-gnu/libpng.so
test/extension/toolbox/test_ext_toolbox_get_num_bits: /usr/lib/x86_64-linux-gnu/libz.so
test/extension/toolbox/test_ext_toolbox_get_num_bits: /usr/lib/x86_64-linux-gnu/libtiff.so
test/extension/toolbox/test_ext_toolbox_get_num_bits: /usr/lib/x86_64-linux-gnu/libtiffxx.so
test/extension/toolbox/test_ext_toolbox_get_num_bits: test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/prathamesh/gil/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test_ext_toolbox_get_num_bits"
	cd /home/prathamesh/gil/test/extension/toolbox && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_ext_toolbox_get_num_bits.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/build: test/extension/toolbox/test_ext_toolbox_get_num_bits

.PHONY : test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/build

test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/clean:
	cd /home/prathamesh/gil/test/extension/toolbox && $(CMAKE_COMMAND) -P CMakeFiles/test_ext_toolbox_get_num_bits.dir/cmake_clean.cmake
.PHONY : test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/clean

test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/depend:
	cd /home/prathamesh/gil && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/prathamesh/gil /home/prathamesh/gil/test/extension/toolbox /home/prathamesh/gil /home/prathamesh/gil/test/extension/toolbox /home/prathamesh/gil/test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/extension/toolbox/CMakeFiles/test_ext_toolbox_get_num_bits.dir/depend

