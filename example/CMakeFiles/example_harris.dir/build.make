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
include example/CMakeFiles/example_harris.dir/depend.make

# Include the progress variables for this target.
include example/CMakeFiles/example_harris.dir/progress.make

# Include the compile flags for this target's objects.
include example/CMakeFiles/example_harris.dir/flags.make

example/CMakeFiles/example_harris.dir/harris.cpp.o: example/CMakeFiles/example_harris.dir/flags.make
example/CMakeFiles/example_harris.dir/harris.cpp.o: example/harris.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/prathamesh/gil/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object example/CMakeFiles/example_harris.dir/harris.cpp.o"
	cd /home/prathamesh/gil/example && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/example_harris.dir/harris.cpp.o -c /home/prathamesh/gil/example/harris.cpp

example/CMakeFiles/example_harris.dir/harris.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/example_harris.dir/harris.cpp.i"
	cd /home/prathamesh/gil/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/prathamesh/gil/example/harris.cpp > CMakeFiles/example_harris.dir/harris.cpp.i

example/CMakeFiles/example_harris.dir/harris.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/example_harris.dir/harris.cpp.s"
	cd /home/prathamesh/gil/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/prathamesh/gil/example/harris.cpp -o CMakeFiles/example_harris.dir/harris.cpp.s

# Object files for target example_harris
example_harris_OBJECTS = \
"CMakeFiles/example_harris.dir/harris.cpp.o"

# External object files for target example_harris
example_harris_EXTERNAL_OBJECTS =

example/example_harris: example/CMakeFiles/example_harris.dir/harris.cpp.o
example/example_harris: example/CMakeFiles/example_harris.dir/build.make
example/example_harris: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so.1.71.0
example/example_harris: /usr/lib/x86_64-linux-gnu/libjpeg.so
example/example_harris: /usr/lib/x86_64-linux-gnu/libpng.so
example/example_harris: /usr/lib/x86_64-linux-gnu/libz.so
example/example_harris: /usr/lib/x86_64-linux-gnu/libtiff.so
example/example_harris: /usr/lib/x86_64-linux-gnu/libtiffxx.so
example/example_harris: example/CMakeFiles/example_harris.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/prathamesh/gil/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable example_harris"
	cd /home/prathamesh/gil/example && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/example_harris.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
example/CMakeFiles/example_harris.dir/build: example/example_harris

.PHONY : example/CMakeFiles/example_harris.dir/build

example/CMakeFiles/example_harris.dir/clean:
	cd /home/prathamesh/gil/example && $(CMAKE_COMMAND) -P CMakeFiles/example_harris.dir/cmake_clean.cmake
.PHONY : example/CMakeFiles/example_harris.dir/clean

example/CMakeFiles/example_harris.dir/depend:
	cd /home/prathamesh/gil && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/prathamesh/gil /home/prathamesh/gil/example /home/prathamesh/gil /home/prathamesh/gil/example /home/prathamesh/gil/example/CMakeFiles/example_harris.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : example/CMakeFiles/example_harris.dir/depend

