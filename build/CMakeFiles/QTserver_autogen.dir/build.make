# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/badr-eddine/QtSocketPlay

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/badr-eddine/QtSocketPlay/build

# Utility rule file for QTserver_autogen.

# Include any custom commands dependencies for this target.
include CMakeFiles/QTserver_autogen.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/QTserver_autogen.dir/progress.make

CMakeFiles/QTserver_autogen: QTserver_autogen/timestamp

QTserver_autogen/timestamp: /usr/lib/qt5/bin/moc
QTserver_autogen/timestamp: CMakeFiles/QTserver_autogen.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/badr-eddine/QtSocketPlay/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Automatic MOC for target QTserver"
	/usr/bin/cmake -E cmake_autogen /home/badr-eddine/QtSocketPlay/build/CMakeFiles/QTserver_autogen.dir/AutogenInfo.json ""
	/usr/bin/cmake -E touch /home/badr-eddine/QtSocketPlay/build/QTserver_autogen/timestamp

QTserver_autogen: CMakeFiles/QTserver_autogen
QTserver_autogen: QTserver_autogen/timestamp
QTserver_autogen: CMakeFiles/QTserver_autogen.dir/build.make
.PHONY : QTserver_autogen

# Rule to build all files generated by this target.
CMakeFiles/QTserver_autogen.dir/build: QTserver_autogen
.PHONY : CMakeFiles/QTserver_autogen.dir/build

CMakeFiles/QTserver_autogen.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/QTserver_autogen.dir/cmake_clean.cmake
.PHONY : CMakeFiles/QTserver_autogen.dir/clean

CMakeFiles/QTserver_autogen.dir/depend:
	cd /home/badr-eddine/QtSocketPlay/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/badr-eddine/QtSocketPlay /home/badr-eddine/QtSocketPlay /home/badr-eddine/QtSocketPlay/build /home/badr-eddine/QtSocketPlay/build /home/badr-eddine/QtSocketPlay/build/CMakeFiles/QTserver_autogen.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/QTserver_autogen.dir/depend

