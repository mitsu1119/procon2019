# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /home/kaito/program/procon2019

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kaito/program/procon2019/build

# Include any dependencies generated for this target.
include src/CMakeFiles/greetings.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/greetings.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/greetings.dir/flags.make

src/CMakeFiles/greetings.dir/agent.cpp.o: src/CMakeFiles/greetings.dir/flags.make
src/CMakeFiles/greetings.dir/agent.cpp.o: ../src/agent.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kaito/program/procon2019/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/greetings.dir/agent.cpp.o"
	cd /home/kaito/program/procon2019/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/greetings.dir/agent.cpp.o -c /home/kaito/program/procon2019/src/agent.cpp

src/CMakeFiles/greetings.dir/agent.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/greetings.dir/agent.cpp.i"
	cd /home/kaito/program/procon2019/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kaito/program/procon2019/src/agent.cpp > CMakeFiles/greetings.dir/agent.cpp.i

src/CMakeFiles/greetings.dir/agent.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/greetings.dir/agent.cpp.s"
	cd /home/kaito/program/procon2019/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kaito/program/procon2019/src/agent.cpp -o CMakeFiles/greetings.dir/agent.cpp.s

src/CMakeFiles/greetings.dir/agent.cpp.o.requires:

.PHONY : src/CMakeFiles/greetings.dir/agent.cpp.o.requires

src/CMakeFiles/greetings.dir/agent.cpp.o.provides: src/CMakeFiles/greetings.dir/agent.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/greetings.dir/build.make src/CMakeFiles/greetings.dir/agent.cpp.o.provides.build
.PHONY : src/CMakeFiles/greetings.dir/agent.cpp.o.provides

src/CMakeFiles/greetings.dir/agent.cpp.o.provides.build: src/CMakeFiles/greetings.dir/agent.cpp.o


src/CMakeFiles/greetings.dir/field.cpp.o: src/CMakeFiles/greetings.dir/flags.make
src/CMakeFiles/greetings.dir/field.cpp.o: ../src/field.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kaito/program/procon2019/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/CMakeFiles/greetings.dir/field.cpp.o"
	cd /home/kaito/program/procon2019/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/greetings.dir/field.cpp.o -c /home/kaito/program/procon2019/src/field.cpp

src/CMakeFiles/greetings.dir/field.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/greetings.dir/field.cpp.i"
	cd /home/kaito/program/procon2019/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kaito/program/procon2019/src/field.cpp > CMakeFiles/greetings.dir/field.cpp.i

src/CMakeFiles/greetings.dir/field.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/greetings.dir/field.cpp.s"
	cd /home/kaito/program/procon2019/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kaito/program/procon2019/src/field.cpp -o CMakeFiles/greetings.dir/field.cpp.s

src/CMakeFiles/greetings.dir/field.cpp.o.requires:

.PHONY : src/CMakeFiles/greetings.dir/field.cpp.o.requires

src/CMakeFiles/greetings.dir/field.cpp.o.provides: src/CMakeFiles/greetings.dir/field.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/greetings.dir/build.make src/CMakeFiles/greetings.dir/field.cpp.o.provides.build
.PHONY : src/CMakeFiles/greetings.dir/field.cpp.o.provides

src/CMakeFiles/greetings.dir/field.cpp.o.provides.build: src/CMakeFiles/greetings.dir/field.cpp.o


src/CMakeFiles/greetings.dir/useful.cpp.o: src/CMakeFiles/greetings.dir/flags.make
src/CMakeFiles/greetings.dir/useful.cpp.o: ../src/useful.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kaito/program/procon2019/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/CMakeFiles/greetings.dir/useful.cpp.o"
	cd /home/kaito/program/procon2019/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/greetings.dir/useful.cpp.o -c /home/kaito/program/procon2019/src/useful.cpp

src/CMakeFiles/greetings.dir/useful.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/greetings.dir/useful.cpp.i"
	cd /home/kaito/program/procon2019/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kaito/program/procon2019/src/useful.cpp > CMakeFiles/greetings.dir/useful.cpp.i

src/CMakeFiles/greetings.dir/useful.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/greetings.dir/useful.cpp.s"
	cd /home/kaito/program/procon2019/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kaito/program/procon2019/src/useful.cpp -o CMakeFiles/greetings.dir/useful.cpp.s

src/CMakeFiles/greetings.dir/useful.cpp.o.requires:

.PHONY : src/CMakeFiles/greetings.dir/useful.cpp.o.requires

src/CMakeFiles/greetings.dir/useful.cpp.o.provides: src/CMakeFiles/greetings.dir/useful.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/greetings.dir/build.make src/CMakeFiles/greetings.dir/useful.cpp.o.provides.build
.PHONY : src/CMakeFiles/greetings.dir/useful.cpp.o.provides

src/CMakeFiles/greetings.dir/useful.cpp.o.provides.build: src/CMakeFiles/greetings.dir/useful.cpp.o


# Object files for target greetings
greetings_OBJECTS = \
"CMakeFiles/greetings.dir/agent.cpp.o" \
"CMakeFiles/greetings.dir/field.cpp.o" \
"CMakeFiles/greetings.dir/useful.cpp.o"

# External object files for target greetings
greetings_EXTERNAL_OBJECTS =

src/libgreetings.so: src/CMakeFiles/greetings.dir/agent.cpp.o
src/libgreetings.so: src/CMakeFiles/greetings.dir/field.cpp.o
src/libgreetings.so: src/CMakeFiles/greetings.dir/useful.cpp.o
src/libgreetings.so: src/CMakeFiles/greetings.dir/build.make
src/libgreetings.so: src/CMakeFiles/greetings.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/kaito/program/procon2019/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX shared library libgreetings.so"
	cd /home/kaito/program/procon2019/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/greetings.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/greetings.dir/build: src/libgreetings.so

.PHONY : src/CMakeFiles/greetings.dir/build

src/CMakeFiles/greetings.dir/requires: src/CMakeFiles/greetings.dir/agent.cpp.o.requires
src/CMakeFiles/greetings.dir/requires: src/CMakeFiles/greetings.dir/field.cpp.o.requires
src/CMakeFiles/greetings.dir/requires: src/CMakeFiles/greetings.dir/useful.cpp.o.requires

.PHONY : src/CMakeFiles/greetings.dir/requires

src/CMakeFiles/greetings.dir/clean:
	cd /home/kaito/program/procon2019/build/src && $(CMAKE_COMMAND) -P CMakeFiles/greetings.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/greetings.dir/clean

src/CMakeFiles/greetings.dir/depend:
	cd /home/kaito/program/procon2019/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kaito/program/procon2019 /home/kaito/program/procon2019/src /home/kaito/program/procon2019/build /home/kaito/program/procon2019/build/src /home/kaito/program/procon2019/build/src/CMakeFiles/greetings.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/greetings.dir/depend

