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
include test/core/color/CMakeFiles/test_core_color_concepts.dir/depend.make

# Include the progress variables for this target.
include test/core/color/CMakeFiles/test_core_color_concepts.dir/progress.make

# Include the compile flags for this target's objects.
include test/core/color/CMakeFiles/test_core_color_concepts.dir/flags.make

test/core/color/CMakeFiles/test_core_color_concepts.dir/concepts.cpp.o: test/core/color/CMakeFiles/test_core_color_concepts.dir/flags.make
test/core/color/CMakeFiles/test_core_color_concepts.dir/concepts.cpp.o: test/core/color/concepts.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/prathamesh/gil/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/core/color/CMakeFiles/test_core_color_concepts.dir/concepts.cpp.o"
	cd /home/prathamesh/gil/test/core/color && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_core_color_concepts.dir/concepts.cpp.o -c /home/prathamesh/gil/test/core/color/concepts.cpp

test/core/color/CMakeFiles/test_core_color_concepts.dir/concepts.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_core_color_concepts.dir/concepts.cpp.i"
	cd /home/prathamesh/gil/test/core/color && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/prathamesh/gil/test/core/color/concepts.cpp > CMakeFiles/test_core_color_concepts.dir/concepts.cpp.i

test/core/color/CMakeFiles/test_core_color_concepts.dir/concepts.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_core_color_concepts.dir/concepts.cpp.s"
	cd /home/prathamesh/gil/test/core/color && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/prathamesh/gil/test/core/color/concepts.cpp -o CMakeFiles/test_core_color_concepts.dir/concepts.cpp.s

# Object files for target test_core_color_concepts
test_core_color_concepts_OBJECTS = \
"CMakeFiles/test_core_color_concepts.dir/concepts.cpp.o"

# External object files for target test_core_color_concepts
test_core_color_concepts_EXTERNAL_OBJECTS =

test/core/color/test_core_color_concepts: test/core/color/CMakeFiles/test_core_color_concepts.dir/concepts.cpp.o
test/core/color/test_core_color_concepts: test/core/color/CMakeFiles/test_core_color_concepts.dir/build.make
test/core/color/test_core_color_concepts: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so.1.71.0
test/core/color/test_core_color_concepts: /usr/lib/x86_64-linux-gnu/libjpeg.so
test/core/color/test_core_color_concepts: /usr/lib/x86_64-linux-gnu/libpng.so
test/core/color/test_core_color_concepts: /usr/lib/x86_64-linux-gnu/libz.so
test/core/color/test_core_color_concepts: /usr/lib/x86_64-linux-gnu/libtiff.so
test/core/color/test_core_color_concepts: /usr/lib/x86_64-linux-gnu/libtiffxx.so
test/core/color/test_core_color_concepts: test/core/color/CMakeFiles/test_core_color_concepts.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/prathamesh/gil/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test_core_color_concepts"
	cd /home/prathamesh/gil/test/core/color && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_core_color_concepts.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/core/color/CMakeFiles/test_core_color_concepts.dir/build: test/core/color/test_core_color_concepts

.PHONY : test/core/color/CMakeFiles/test_core_color_concepts.dir/build

test/core/color/CMakeFiles/test_core_color_concepts.dir/clean:
	cd /home/prathamesh/gil/test/core/color && $(CMAKE_COMMAND) -P CMakeFiles/test_core_color_concepts.dir/cmake_clean.cmake
.PHONY : test/core/color/CMakeFiles/test_core_color_concepts.dir/clean

test/core/color/CMakeFiles/test_core_color_concepts.dir/depend:
	cd /home/prathamesh/gil && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/prathamesh/gil /home/prathamesh/gil/test/core/color /home/prathamesh/gil /home/prathamesh/gil/test/core/color /home/prathamesh/gil/test/core/color/CMakeFiles/test_core_color_concepts.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/core/color/CMakeFiles/test_core_color_concepts.dir/depend

