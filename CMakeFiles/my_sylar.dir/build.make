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


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_SOURCE_DIR = /home/jerry/mydir/project_server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jerry/mydir/project_server

# Include any dependencies generated for this target.
include CMakeFiles/my_sylar.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/my_sylar.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/my_sylar.dir/flags.make

CMakeFiles/my_sylar.dir/my_sylar/log.cpp.o: CMakeFiles/my_sylar.dir/flags.make
CMakeFiles/my_sylar.dir/my_sylar/log.cpp.o: my_sylar/log.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jerry/mydir/project_server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/my_sylar.dir/my_sylar/log.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/my_sylar.dir/my_sylar/log.cpp.o -c /home/jerry/mydir/project_server/my_sylar/log.cpp

CMakeFiles/my_sylar.dir/my_sylar/log.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/my_sylar.dir/my_sylar/log.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jerry/mydir/project_server/my_sylar/log.cpp > CMakeFiles/my_sylar.dir/my_sylar/log.cpp.i

CMakeFiles/my_sylar.dir/my_sylar/log.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/my_sylar.dir/my_sylar/log.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jerry/mydir/project_server/my_sylar/log.cpp -o CMakeFiles/my_sylar.dir/my_sylar/log.cpp.s

CMakeFiles/my_sylar.dir/my_sylar/util.cpp.o: CMakeFiles/my_sylar.dir/flags.make
CMakeFiles/my_sylar.dir/my_sylar/util.cpp.o: my_sylar/util.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jerry/mydir/project_server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/my_sylar.dir/my_sylar/util.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/my_sylar.dir/my_sylar/util.cpp.o -c /home/jerry/mydir/project_server/my_sylar/util.cpp

CMakeFiles/my_sylar.dir/my_sylar/util.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/my_sylar.dir/my_sylar/util.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jerry/mydir/project_server/my_sylar/util.cpp > CMakeFiles/my_sylar.dir/my_sylar/util.cpp.i

CMakeFiles/my_sylar.dir/my_sylar/util.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/my_sylar.dir/my_sylar/util.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jerry/mydir/project_server/my_sylar/util.cpp -o CMakeFiles/my_sylar.dir/my_sylar/util.cpp.s

CMakeFiles/my_sylar.dir/my_sylar/config.cpp.o: CMakeFiles/my_sylar.dir/flags.make
CMakeFiles/my_sylar.dir/my_sylar/config.cpp.o: my_sylar/config.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jerry/mydir/project_server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/my_sylar.dir/my_sylar/config.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/my_sylar.dir/my_sylar/config.cpp.o -c /home/jerry/mydir/project_server/my_sylar/config.cpp

CMakeFiles/my_sylar.dir/my_sylar/config.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/my_sylar.dir/my_sylar/config.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jerry/mydir/project_server/my_sylar/config.cpp > CMakeFiles/my_sylar.dir/my_sylar/config.cpp.i

CMakeFiles/my_sylar.dir/my_sylar/config.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/my_sylar.dir/my_sylar/config.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jerry/mydir/project_server/my_sylar/config.cpp -o CMakeFiles/my_sylar.dir/my_sylar/config.cpp.s

CMakeFiles/my_sylar.dir/my_sylar/thread.cpp.o: CMakeFiles/my_sylar.dir/flags.make
CMakeFiles/my_sylar.dir/my_sylar/thread.cpp.o: my_sylar/thread.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jerry/mydir/project_server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/my_sylar.dir/my_sylar/thread.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/my_sylar.dir/my_sylar/thread.cpp.o -c /home/jerry/mydir/project_server/my_sylar/thread.cpp

CMakeFiles/my_sylar.dir/my_sylar/thread.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/my_sylar.dir/my_sylar/thread.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jerry/mydir/project_server/my_sylar/thread.cpp > CMakeFiles/my_sylar.dir/my_sylar/thread.cpp.i

CMakeFiles/my_sylar.dir/my_sylar/thread.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/my_sylar.dir/my_sylar/thread.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jerry/mydir/project_server/my_sylar/thread.cpp -o CMakeFiles/my_sylar.dir/my_sylar/thread.cpp.s

CMakeFiles/my_sylar.dir/my_sylar/fiber.cpp.o: CMakeFiles/my_sylar.dir/flags.make
CMakeFiles/my_sylar.dir/my_sylar/fiber.cpp.o: my_sylar/fiber.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jerry/mydir/project_server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/my_sylar.dir/my_sylar/fiber.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/my_sylar.dir/my_sylar/fiber.cpp.o -c /home/jerry/mydir/project_server/my_sylar/fiber.cpp

CMakeFiles/my_sylar.dir/my_sylar/fiber.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/my_sylar.dir/my_sylar/fiber.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jerry/mydir/project_server/my_sylar/fiber.cpp > CMakeFiles/my_sylar.dir/my_sylar/fiber.cpp.i

CMakeFiles/my_sylar.dir/my_sylar/fiber.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/my_sylar.dir/my_sylar/fiber.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jerry/mydir/project_server/my_sylar/fiber.cpp -o CMakeFiles/my_sylar.dir/my_sylar/fiber.cpp.s

CMakeFiles/my_sylar.dir/my_sylar/scheduler.cpp.o: CMakeFiles/my_sylar.dir/flags.make
CMakeFiles/my_sylar.dir/my_sylar/scheduler.cpp.o: my_sylar/scheduler.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jerry/mydir/project_server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/my_sylar.dir/my_sylar/scheduler.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/my_sylar.dir/my_sylar/scheduler.cpp.o -c /home/jerry/mydir/project_server/my_sylar/scheduler.cpp

CMakeFiles/my_sylar.dir/my_sylar/scheduler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/my_sylar.dir/my_sylar/scheduler.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jerry/mydir/project_server/my_sylar/scheduler.cpp > CMakeFiles/my_sylar.dir/my_sylar/scheduler.cpp.i

CMakeFiles/my_sylar.dir/my_sylar/scheduler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/my_sylar.dir/my_sylar/scheduler.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jerry/mydir/project_server/my_sylar/scheduler.cpp -o CMakeFiles/my_sylar.dir/my_sylar/scheduler.cpp.s

CMakeFiles/my_sylar.dir/my_sylar/iomanager.cpp.o: CMakeFiles/my_sylar.dir/flags.make
CMakeFiles/my_sylar.dir/my_sylar/iomanager.cpp.o: my_sylar/iomanager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jerry/mydir/project_server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/my_sylar.dir/my_sylar/iomanager.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/my_sylar.dir/my_sylar/iomanager.cpp.o -c /home/jerry/mydir/project_server/my_sylar/iomanager.cpp

CMakeFiles/my_sylar.dir/my_sylar/iomanager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/my_sylar.dir/my_sylar/iomanager.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jerry/mydir/project_server/my_sylar/iomanager.cpp > CMakeFiles/my_sylar.dir/my_sylar/iomanager.cpp.i

CMakeFiles/my_sylar.dir/my_sylar/iomanager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/my_sylar.dir/my_sylar/iomanager.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jerry/mydir/project_server/my_sylar/iomanager.cpp -o CMakeFiles/my_sylar.dir/my_sylar/iomanager.cpp.s

CMakeFiles/my_sylar.dir/my_sylar/timer.cpp.o: CMakeFiles/my_sylar.dir/flags.make
CMakeFiles/my_sylar.dir/my_sylar/timer.cpp.o: my_sylar/timer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jerry/mydir/project_server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/my_sylar.dir/my_sylar/timer.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/my_sylar.dir/my_sylar/timer.cpp.o -c /home/jerry/mydir/project_server/my_sylar/timer.cpp

CMakeFiles/my_sylar.dir/my_sylar/timer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/my_sylar.dir/my_sylar/timer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jerry/mydir/project_server/my_sylar/timer.cpp > CMakeFiles/my_sylar.dir/my_sylar/timer.cpp.i

CMakeFiles/my_sylar.dir/my_sylar/timer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/my_sylar.dir/my_sylar/timer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jerry/mydir/project_server/my_sylar/timer.cpp -o CMakeFiles/my_sylar.dir/my_sylar/timer.cpp.s

CMakeFiles/my_sylar.dir/my_sylar/hook.cpp.o: CMakeFiles/my_sylar.dir/flags.make
CMakeFiles/my_sylar.dir/my_sylar/hook.cpp.o: my_sylar/hook.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jerry/mydir/project_server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/my_sylar.dir/my_sylar/hook.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/my_sylar.dir/my_sylar/hook.cpp.o -c /home/jerry/mydir/project_server/my_sylar/hook.cpp

CMakeFiles/my_sylar.dir/my_sylar/hook.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/my_sylar.dir/my_sylar/hook.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jerry/mydir/project_server/my_sylar/hook.cpp > CMakeFiles/my_sylar.dir/my_sylar/hook.cpp.i

CMakeFiles/my_sylar.dir/my_sylar/hook.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/my_sylar.dir/my_sylar/hook.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jerry/mydir/project_server/my_sylar/hook.cpp -o CMakeFiles/my_sylar.dir/my_sylar/hook.cpp.s

# Object files for target my_sylar
my_sylar_OBJECTS = \
"CMakeFiles/my_sylar.dir/my_sylar/log.cpp.o" \
"CMakeFiles/my_sylar.dir/my_sylar/util.cpp.o" \
"CMakeFiles/my_sylar.dir/my_sylar/config.cpp.o" \
"CMakeFiles/my_sylar.dir/my_sylar/thread.cpp.o" \
"CMakeFiles/my_sylar.dir/my_sylar/fiber.cpp.o" \
"CMakeFiles/my_sylar.dir/my_sylar/scheduler.cpp.o" \
"CMakeFiles/my_sylar.dir/my_sylar/iomanager.cpp.o" \
"CMakeFiles/my_sylar.dir/my_sylar/timer.cpp.o" \
"CMakeFiles/my_sylar.dir/my_sylar/hook.cpp.o"

# External object files for target my_sylar
my_sylar_EXTERNAL_OBJECTS =

lib/libmy_sylar.so: CMakeFiles/my_sylar.dir/my_sylar/log.cpp.o
lib/libmy_sylar.so: CMakeFiles/my_sylar.dir/my_sylar/util.cpp.o
lib/libmy_sylar.so: CMakeFiles/my_sylar.dir/my_sylar/config.cpp.o
lib/libmy_sylar.so: CMakeFiles/my_sylar.dir/my_sylar/thread.cpp.o
lib/libmy_sylar.so: CMakeFiles/my_sylar.dir/my_sylar/fiber.cpp.o
lib/libmy_sylar.so: CMakeFiles/my_sylar.dir/my_sylar/scheduler.cpp.o
lib/libmy_sylar.so: CMakeFiles/my_sylar.dir/my_sylar/iomanager.cpp.o
lib/libmy_sylar.so: CMakeFiles/my_sylar.dir/my_sylar/timer.cpp.o
lib/libmy_sylar.so: CMakeFiles/my_sylar.dir/my_sylar/hook.cpp.o
lib/libmy_sylar.so: CMakeFiles/my_sylar.dir/build.make
lib/libmy_sylar.so: CMakeFiles/my_sylar.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jerry/mydir/project_server/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Linking CXX shared library lib/libmy_sylar.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/my_sylar.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/my_sylar.dir/build: lib/libmy_sylar.so

.PHONY : CMakeFiles/my_sylar.dir/build

CMakeFiles/my_sylar.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/my_sylar.dir/cmake_clean.cmake
.PHONY : CMakeFiles/my_sylar.dir/clean

CMakeFiles/my_sylar.dir/depend:
	cd /home/jerry/mydir/project_server && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jerry/mydir/project_server /home/jerry/mydir/project_server /home/jerry/mydir/project_server /home/jerry/mydir/project_server /home/jerry/mydir/project_server/CMakeFiles/my_sylar.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/my_sylar.dir/depend

