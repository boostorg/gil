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
include test/legacy/CMakeFiles/test_legacy_channel.dir/depend.make

# Include the progress variables for this target.
include test/legacy/CMakeFiles/test_legacy_channel.dir/progress.make

# Include the compile flags for this target's objects.
include test/legacy/CMakeFiles/test_legacy_channel.dir/flags.make

test/legacy/CMakeFiles/test_legacy_channel.dir/channel.cpp.o: test/legacy/CMakeFiles/test_legacy_channel.dir/flags.make
test/legacy/CMakeFiles/test_legacy_channel.dir/channel.cpp.o: test/legacy/channel.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/prathamesh/gil/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/legacy/CMakeFiles/test_legacy_channel.dir/channel.cpp.o"
	cd /home/prathamesh/gil/test/legacy && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_legacy_channel.dir/channel.cpp.o -c /home/prathamesh/gil/test/legacy/channel.cpp

test/legacy/CMakeFiles/test_legacy_channel.dir/channel.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_legacy_channel.dir/channel.cpp.i"
	cd /home/prathamesh/gil/test/legacy && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/prathamesh/gil/test/legacy/channel.cpp > CMakeFiles/test_legacy_channel.dir/channel.cpp.i

test/legacy/CMakeFiles/test_legacy_channel.dir/channel.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_legacy_channel.dir/channel.cpp.s"
	cd /home/prathamesh/gil/test/legacy && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/prathamesh/gil/test/legacy/channel.cpp -o CMakeFiles/test_legacy_channel.dir/channel.cpp.s

test/legacy/CMakeFiles/test_legacy_channel.dir/error_if.cpp.o: test/legacy/CMakeFiles/test_legacy_channel.dir/flags.make
test/legacy/CMakeFiles/test_legacy_channel.dir/error_if.cpp.o: test/legacy/error_if.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/prathamesh/gil/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object test/legacy/CMakeFiles/test_legacy_channel.dir/error_if.cpp.o"
	cd /home/prathamesh/gil/test/legacy && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_legacy_channel.dir/error_if.cpp.o -c /home/prathamesh/gil/test/legacy/error_if.cpp

test/legacy/CMakeFiles/test_legacy_channel.dir/error_if.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_legacy_channel.dir/error_if.cpp.i"
	cd /home/prathamesh/gil/test/legacy && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/prathamesh/gil/test/legacy/error_if.cpp > CMakeFiles/test_legacy_channel.dir/error_if.cpp.i

test/legacy/CMakeFiles/test_legacy_channel.dir/error_if.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_legacy_channel.dir/error_if.cpp.s"
	cd /home/prathamesh/gil/test/legacy && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/prathamesh/gil/test/legacy/error_if.cpp -o CMakeFiles/test_legacy_channel.dir/error_if.cpp.s

# Object files for target test_legacy_channel
test_legacy_channel_OBJECTS = \
"CMakeFiles/test_legacy_channel.dir/channel.cpp.o" \
"CMakeFiles/test_legacy_channel.dir/error_if.cpp.o"

# External object files for target test_legacy_channel
test_legacy_channel_EXTERNAL_OBJECTS =

test/legacy/test_legacy_channel: test/legacy/CMakeFiles/test_legacy_channel.dir/channel.cpp.o
test/legacy/test_legacy_channel: test/legacy/CMakeFiles/test_legacy_channel.dir/error_if.cpp.o
test/legacy/test_legacy_channel: test/legacy/CMakeFiles/test_legacy_channel.dir/build.make
test/legacy/test_legacy_channel: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so.1.71.0
test/legacy/test_legacy_channel: /usr/lib/x86_64-linux-gnu/libjpeg.so
test/legacy/test_legacy_channel: /usr/lib/x86_64-linux-gnu/libpng.so
test/legacy/test_legacy_channel: /usr/lib/x86_64-linux-gnu/libz.so
test/legacy/test_legacy_channel: /usr/lib/x86_64-linux-gnu/libtiff.so
test/legacy/test_legacy_channel: /usr/lib/x86_64-linux-gnu/libtiffxx.so
test/legacy/test_legacy_channel: test/legacy/CMakeFiles/test_legacy_channel.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/prathamesh/gil/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable test_legacy_channel"
	cd /home/prathamesh/gil/test/legacy && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_legacy_channel.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/legacy/CMakeFiles/test_legacy_channel.dir/build: test/legacy/test_legacy_channel

.PHONY : test/legacy/CMakeFiles/test_legacy_channel.dir/build

test/legacy/CMakeFiles/test_legacy_channel.dir/clean:
	cd /home/prathamesh/gil/test/legacy && $(CMAKE_COMMAND) -P CMakeFiles/test_legacy_channel.dir/cmake_clean.cmake
.PHONY : test/legacy/CMakeFiles/test_legacy_channel.dir/clean

test/legacy/CMakeFiles/test_legacy_channel.dir/depend:
	cd /home/prathamesh/gil && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/prathamesh/gil /home/prathamesh/gil/test/legacy /home/prathamesh/gil /home/prathamesh/gil/test/legacy /home/prathamesh/gil/test/legacy/CMakeFiles/test_legacy_channel.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/legacy/CMakeFiles/test_legacy_channel.dir/depend

