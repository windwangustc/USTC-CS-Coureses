# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/chen/Desktop/C++面向对象技术/Exp4_GUI

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/chen/Desktop/C++面向对象技术/Exp4_GUI/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Exp4_GUI.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Exp4_GUI.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Exp4_GUI.dir/flags.make

CMakeFiles/Exp4_GUI.dir/main.cpp.o: CMakeFiles/Exp4_GUI.dir/flags.make
CMakeFiles/Exp4_GUI.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/chen/Desktop/C++面向对象技术/Exp4_GUI/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Exp4_GUI.dir/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Exp4_GUI.dir/main.cpp.o -c /Users/chen/Desktop/C++面向对象技术/Exp4_GUI/main.cpp

CMakeFiles/Exp4_GUI.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Exp4_GUI.dir/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/chen/Desktop/C++面向对象技术/Exp4_GUI/main.cpp > CMakeFiles/Exp4_GUI.dir/main.cpp.i

CMakeFiles/Exp4_GUI.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Exp4_GUI.dir/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/chen/Desktop/C++面向对象技术/Exp4_GUI/main.cpp -o CMakeFiles/Exp4_GUI.dir/main.cpp.s

# Object files for target Exp4_GUI
Exp4_GUI_OBJECTS = \
"CMakeFiles/Exp4_GUI.dir/main.cpp.o"

# External object files for target Exp4_GUI
Exp4_GUI_EXTERNAL_OBJECTS =

Exp4_GUI: CMakeFiles/Exp4_GUI.dir/main.cpp.o
Exp4_GUI: CMakeFiles/Exp4_GUI.dir/build.make
Exp4_GUI: CMakeFiles/Exp4_GUI.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/chen/Desktop/C++面向对象技术/Exp4_GUI/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Exp4_GUI"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Exp4_GUI.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Exp4_GUI.dir/build: Exp4_GUI

.PHONY : CMakeFiles/Exp4_GUI.dir/build

CMakeFiles/Exp4_GUI.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Exp4_GUI.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Exp4_GUI.dir/clean

CMakeFiles/Exp4_GUI.dir/depend:
	cd /Users/chen/Desktop/C++面向对象技术/Exp4_GUI/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/chen/Desktop/C++面向对象技术/Exp4_GUI /Users/chen/Desktop/C++面向对象技术/Exp4_GUI /Users/chen/Desktop/C++面向对象技术/Exp4_GUI/cmake-build-debug /Users/chen/Desktop/C++面向对象技术/Exp4_GUI/cmake-build-debug /Users/chen/Desktop/C++面向对象技术/Exp4_GUI/cmake-build-debug/CMakeFiles/Exp4_GUI.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Exp4_GUI.dir/depend

