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
include test/header/CMakeFiles/test_header_promote_integral.dir/depend.make

# Include the progress variables for this target.
include test/header/CMakeFiles/test_header_promote_integral.dir/progress.make

# Include the compile flags for this target's objects.
include test/header/CMakeFiles/test_header_promote_integral.dir/flags.make

test/header/CMakeFiles/test_header_promote_integral.dir/__/headers/promote_integral.cpp.o: test/header/CMakeFiles/test_header_promote_integral.dir/flags.make
test/header/CMakeFiles/test_header_promote_integral.dir/__/headers/promote_integral.cpp.o: test/headers/promote_integral.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/prathamesh/gil/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/header/CMakeFiles/test_header_promote_integral.dir/__/headers/promote_integral.cpp.o"
	cd /home/prathamesh/gil/test/header && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_header_promote_integral.dir/__/headers/promote_integral.cpp.o -c /home/prathamesh/gil/test/headers/promote_integral.cpp

test/header/CMakeFiles/test_header_promote_integral.dir/__/headers/promote_integral.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_header_promote_integral.dir/__/headers/promote_integral.cpp.i"
	cd /home/prathamesh/gil/test/header && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/prathamesh/gil/test/headers/promote_integral.cpp > CMakeFiles/test_header_promote_integral.dir/__/headers/promote_integral.cpp.i

test/header/CMakeFiles/test_header_promote_integral.dir/__/headers/promote_integral.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_header_promote_integral.dir/__/headers/promote_integral.cpp.s"
	cd /home/prathamesh/gil/test/header && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/prathamesh/gil/test/headers/promote_integral.cpp -o CMakeFiles/test_header_promote_integral.dir/__/headers/promote_integral.cpp.s

# Object files for target test_header_promote_integral
test_header_promote_integral_OBJECTS = \
"CMakeFiles/test_header_promote_integral.dir/__/headers/promote_integral.cpp.o"

# External object files for target test_header_promote_integral
test_header_promote_integral_EXTERNAL_OBJECTS =

test/header/test_header_promote_integral: test/header/CMakeFiles/test_header_promote_integral.dir/__/headers/promote_integral.cpp.o
test/header/test_header_promote_integral: test/header/CMakeFiles/test_header_promote_integral.dir/build.make
test/header/test_header_promote_integral: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so.1.71.0
test/header/test_header_promote_integral: /usr/lib/x86_64-linux-gnu/libjpeg.so
test/header/test_header_promote_integral: /usr/lib/x86_64-linux-gnu/libpng.so
test/header/test_header_promote_integral: /usr/lib/x86_64-linux-gnu/libz.so
test/header/test_header_promote_integral: /usr/lib/x86_64-linux-gnu/libtiff.so
test/header/test_header_promote_integral: /usr/lib/x86_64-linux-gnu/libtiffxx.so
test/header/test_header_promote_integral: test/header/CMakeFiles/test_header_promote_integral.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/prathamesh/gil/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test_header_promote_integral"
	cd /home/prathamesh/gil/test/header && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_header_promote_integral.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/header/CMakeFiles/test_header_promote_integral.dir/build: test/header/test_header_promote_integral

.PHONY : test/header/CMakeFiles/test_header_promote_integral.dir/build

test/header/CMakeFiles/test_header_promote_integral.dir/clean:
	cd /home/prathamesh/gil/test/header && $(CMAKE_COMMAND) -P CMakeFiles/test_header_promote_integral.dir/cmake_clean.cmake
.PHONY : test/header/CMakeFiles/test_header_promote_integral.dir/clean

test/header/CMakeFiles/test_header_promote_integral.dir/depend:
	cd /home/prathamesh/gil && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/prathamesh/gil /home/prathamesh/gil/test/header /home/prathamesh/gil /home/prathamesh/gil/test/header /home/prathamesh/gil/test/header/CMakeFiles/test_header_promote_integral.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/header/CMakeFiles/test_header_promote_integral.dir/depend

