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
include test/core/image/CMakeFiles/test_core_image_image.dir/depend.make

# Include the progress variables for this target.
include test/core/image/CMakeFiles/test_core_image_image.dir/progress.make

# Include the compile flags for this target's objects.
include test/core/image/CMakeFiles/test_core_image_image.dir/flags.make

test/core/image/CMakeFiles/test_core_image_image.dir/image.cpp.o: test/core/image/CMakeFiles/test_core_image_image.dir/flags.make
test/core/image/CMakeFiles/test_core_image_image.dir/image.cpp.o: test/core/image/image.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/prathamesh/gil/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/core/image/CMakeFiles/test_core_image_image.dir/image.cpp.o"
	cd /home/prathamesh/gil/test/core/image && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_core_image_image.dir/image.cpp.o -c /home/prathamesh/gil/test/core/image/image.cpp

test/core/image/CMakeFiles/test_core_image_image.dir/image.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_core_image_image.dir/image.cpp.i"
	cd /home/prathamesh/gil/test/core/image && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/prathamesh/gil/test/core/image/image.cpp > CMakeFiles/test_core_image_image.dir/image.cpp.i

test/core/image/CMakeFiles/test_core_image_image.dir/image.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_core_image_image.dir/image.cpp.s"
	cd /home/prathamesh/gil/test/core/image && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/prathamesh/gil/test/core/image/image.cpp -o CMakeFiles/test_core_image_image.dir/image.cpp.s

# Object files for target test_core_image_image
test_core_image_image_OBJECTS = \
"CMakeFiles/test_core_image_image.dir/image.cpp.o"

# External object files for target test_core_image_image
test_core_image_image_EXTERNAL_OBJECTS =

test/core/image/test_core_image_image: test/core/image/CMakeFiles/test_core_image_image.dir/image.cpp.o
test/core/image/test_core_image_image: test/core/image/CMakeFiles/test_core_image_image.dir/build.make
test/core/image/test_core_image_image: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so.1.71.0
test/core/image/test_core_image_image: /usr/lib/x86_64-linux-gnu/libjpeg.so
test/core/image/test_core_image_image: /usr/lib/x86_64-linux-gnu/libpng.so
test/core/image/test_core_image_image: /usr/lib/x86_64-linux-gnu/libz.so
test/core/image/test_core_image_image: /usr/lib/x86_64-linux-gnu/libtiff.so
test/core/image/test_core_image_image: /usr/lib/x86_64-linux-gnu/libtiffxx.so
test/core/image/test_core_image_image: test/core/image/CMakeFiles/test_core_image_image.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/prathamesh/gil/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test_core_image_image"
	cd /home/prathamesh/gil/test/core/image && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_core_image_image.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/core/image/CMakeFiles/test_core_image_image.dir/build: test/core/image/test_core_image_image

.PHONY : test/core/image/CMakeFiles/test_core_image_image.dir/build

test/core/image/CMakeFiles/test_core_image_image.dir/clean:
	cd /home/prathamesh/gil/test/core/image && $(CMAKE_COMMAND) -P CMakeFiles/test_core_image_image.dir/cmake_clean.cmake
.PHONY : test/core/image/CMakeFiles/test_core_image_image.dir/clean

test/core/image/CMakeFiles/test_core_image_image.dir/depend:
	cd /home/prathamesh/gil && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/prathamesh/gil /home/prathamesh/gil/test/core/image /home/prathamesh/gil /home/prathamesh/gil/test/core/image /home/prathamesh/gil/test/core/image/CMakeFiles/test_core_image_image.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/core/image/CMakeFiles/test_core_image_image.dir/depend

