# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/jerry/mydir/project_server/CMakeFiles /home/jerry/mydir/project_server/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/jerry/mydir/project_server/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named test_scheduler

# Build rule for target.
test_scheduler: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 test_scheduler
.PHONY : test_scheduler

# fast build rule for target.
test_scheduler/fast:
	$(MAKE) -f CMakeFiles/test_scheduler.dir/build.make CMakeFiles/test_scheduler.dir/build
.PHONY : test_scheduler/fast

#=============================================================================
# Target rules for targets named test_config

# Build rule for target.
test_config: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 test_config
.PHONY : test_config

# fast build rule for target.
test_config/fast:
	$(MAKE) -f CMakeFiles/test_config.dir/build.make CMakeFiles/test_config.dir/build
.PHONY : test_config/fast

#=============================================================================
# Target rules for targets named my_sylar

# Build rule for target.
my_sylar: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 my_sylar
.PHONY : my_sylar

# fast build rule for target.
my_sylar/fast:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/build
.PHONY : my_sylar/fast

#=============================================================================
# Target rules for targets named test_bytearray

# Build rule for target.
test_bytearray: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 test_bytearray
.PHONY : test_bytearray

# fast build rule for target.
test_bytearray/fast:
	$(MAKE) -f CMakeFiles/test_bytearray.dir/build.make CMakeFiles/test_bytearray.dir/build
.PHONY : test_bytearray/fast

#=============================================================================
# Target rules for targets named test_thread

# Build rule for target.
test_thread: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 test_thread
.PHONY : test_thread

# fast build rule for target.
test_thread/fast:
	$(MAKE) -f CMakeFiles/test_thread.dir/build.make CMakeFiles/test_thread.dir/build
.PHONY : test_thread/fast

#=============================================================================
# Target rules for targets named test_util

# Build rule for target.
test_util: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 test_util
.PHONY : test_util

# fast build rule for target.
test_util/fast:
	$(MAKE) -f CMakeFiles/test_util.dir/build.make CMakeFiles/test_util.dir/build
.PHONY : test_util/fast

#=============================================================================
# Target rules for targets named test_fiber

# Build rule for target.
test_fiber: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 test_fiber
.PHONY : test_fiber

# fast build rule for target.
test_fiber/fast:
	$(MAKE) -f CMakeFiles/test_fiber.dir/build.make CMakeFiles/test_fiber.dir/build
.PHONY : test_fiber/fast

#=============================================================================
# Target rules for targets named test_log

# Build rule for target.
test_log: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 test_log
.PHONY : test_log

# fast build rule for target.
test_log/fast:
	$(MAKE) -f CMakeFiles/test_log.dir/build.make CMakeFiles/test_log.dir/build
.PHONY : test_log/fast

#=============================================================================
# Target rules for targets named test_address

# Build rule for target.
test_address: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 test_address
.PHONY : test_address

# fast build rule for target.
test_address/fast:
	$(MAKE) -f CMakeFiles/test_address.dir/build.make CMakeFiles/test_address.dir/build
.PHONY : test_address/fast

#=============================================================================
# Target rules for targets named test_iomanager

# Build rule for target.
test_iomanager: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 test_iomanager
.PHONY : test_iomanager

# fast build rule for target.
test_iomanager/fast:
	$(MAKE) -f CMakeFiles/test_iomanager.dir/build.make CMakeFiles/test_iomanager.dir/build
.PHONY : test_iomanager/fast

#=============================================================================
# Target rules for targets named test_hook

# Build rule for target.
test_hook: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 test_hook
.PHONY : test_hook

# fast build rule for target.
test_hook/fast:
	$(MAKE) -f CMakeFiles/test_hook.dir/build.make CMakeFiles/test_hook.dir/build
.PHONY : test_hook/fast

#=============================================================================
# Target rules for targets named test_socket

# Build rule for target.
test_socket: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 test_socket
.PHONY : test_socket

# fast build rule for target.
test_socket/fast:
	$(MAKE) -f CMakeFiles/test_socket.dir/build.make CMakeFiles/test_socket.dir/build
.PHONY : test_socket/fast

my_sylar/address.o: my_sylar/address.cpp.o

.PHONY : my_sylar/address.o

# target to build an object file
my_sylar/address.cpp.o:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/address.cpp.o
.PHONY : my_sylar/address.cpp.o

my_sylar/address.i: my_sylar/address.cpp.i

.PHONY : my_sylar/address.i

# target to preprocess a source file
my_sylar/address.cpp.i:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/address.cpp.i
.PHONY : my_sylar/address.cpp.i

my_sylar/address.s: my_sylar/address.cpp.s

.PHONY : my_sylar/address.s

# target to generate assembly for a file
my_sylar/address.cpp.s:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/address.cpp.s
.PHONY : my_sylar/address.cpp.s

my_sylar/bytearray.o: my_sylar/bytearray.cpp.o

.PHONY : my_sylar/bytearray.o

# target to build an object file
my_sylar/bytearray.cpp.o:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/bytearray.cpp.o
.PHONY : my_sylar/bytearray.cpp.o

my_sylar/bytearray.i: my_sylar/bytearray.cpp.i

.PHONY : my_sylar/bytearray.i

# target to preprocess a source file
my_sylar/bytearray.cpp.i:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/bytearray.cpp.i
.PHONY : my_sylar/bytearray.cpp.i

my_sylar/bytearray.s: my_sylar/bytearray.cpp.s

.PHONY : my_sylar/bytearray.s

# target to generate assembly for a file
my_sylar/bytearray.cpp.s:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/bytearray.cpp.s
.PHONY : my_sylar/bytearray.cpp.s

my_sylar/config.o: my_sylar/config.cpp.o

.PHONY : my_sylar/config.o

# target to build an object file
my_sylar/config.cpp.o:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/config.cpp.o
.PHONY : my_sylar/config.cpp.o

my_sylar/config.i: my_sylar/config.cpp.i

.PHONY : my_sylar/config.i

# target to preprocess a source file
my_sylar/config.cpp.i:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/config.cpp.i
.PHONY : my_sylar/config.cpp.i

my_sylar/config.s: my_sylar/config.cpp.s

.PHONY : my_sylar/config.s

# target to generate assembly for a file
my_sylar/config.cpp.s:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/config.cpp.s
.PHONY : my_sylar/config.cpp.s

my_sylar/fd_manager.o: my_sylar/fd_manager.cpp.o

.PHONY : my_sylar/fd_manager.o

# target to build an object file
my_sylar/fd_manager.cpp.o:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/fd_manager.cpp.o
.PHONY : my_sylar/fd_manager.cpp.o

my_sylar/fd_manager.i: my_sylar/fd_manager.cpp.i

.PHONY : my_sylar/fd_manager.i

# target to preprocess a source file
my_sylar/fd_manager.cpp.i:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/fd_manager.cpp.i
.PHONY : my_sylar/fd_manager.cpp.i

my_sylar/fd_manager.s: my_sylar/fd_manager.cpp.s

.PHONY : my_sylar/fd_manager.s

# target to generate assembly for a file
my_sylar/fd_manager.cpp.s:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/fd_manager.cpp.s
.PHONY : my_sylar/fd_manager.cpp.s

my_sylar/fiber.o: my_sylar/fiber.cpp.o

.PHONY : my_sylar/fiber.o

# target to build an object file
my_sylar/fiber.cpp.o:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/fiber.cpp.o
.PHONY : my_sylar/fiber.cpp.o

my_sylar/fiber.i: my_sylar/fiber.cpp.i

.PHONY : my_sylar/fiber.i

# target to preprocess a source file
my_sylar/fiber.cpp.i:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/fiber.cpp.i
.PHONY : my_sylar/fiber.cpp.i

my_sylar/fiber.s: my_sylar/fiber.cpp.s

.PHONY : my_sylar/fiber.s

# target to generate assembly for a file
my_sylar/fiber.cpp.s:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/fiber.cpp.s
.PHONY : my_sylar/fiber.cpp.s

my_sylar/hook.o: my_sylar/hook.cpp.o

.PHONY : my_sylar/hook.o

# target to build an object file
my_sylar/hook.cpp.o:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/hook.cpp.o
.PHONY : my_sylar/hook.cpp.o

my_sylar/hook.i: my_sylar/hook.cpp.i

.PHONY : my_sylar/hook.i

# target to preprocess a source file
my_sylar/hook.cpp.i:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/hook.cpp.i
.PHONY : my_sylar/hook.cpp.i

my_sylar/hook.s: my_sylar/hook.cpp.s

.PHONY : my_sylar/hook.s

# target to generate assembly for a file
my_sylar/hook.cpp.s:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/hook.cpp.s
.PHONY : my_sylar/hook.cpp.s

my_sylar/iomanager.o: my_sylar/iomanager.cpp.o

.PHONY : my_sylar/iomanager.o

# target to build an object file
my_sylar/iomanager.cpp.o:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/iomanager.cpp.o
.PHONY : my_sylar/iomanager.cpp.o

my_sylar/iomanager.i: my_sylar/iomanager.cpp.i

.PHONY : my_sylar/iomanager.i

# target to preprocess a source file
my_sylar/iomanager.cpp.i:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/iomanager.cpp.i
.PHONY : my_sylar/iomanager.cpp.i

my_sylar/iomanager.s: my_sylar/iomanager.cpp.s

.PHONY : my_sylar/iomanager.s

# target to generate assembly for a file
my_sylar/iomanager.cpp.s:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/iomanager.cpp.s
.PHONY : my_sylar/iomanager.cpp.s

my_sylar/log.o: my_sylar/log.cpp.o

.PHONY : my_sylar/log.o

# target to build an object file
my_sylar/log.cpp.o:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/log.cpp.o
.PHONY : my_sylar/log.cpp.o

my_sylar/log.i: my_sylar/log.cpp.i

.PHONY : my_sylar/log.i

# target to preprocess a source file
my_sylar/log.cpp.i:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/log.cpp.i
.PHONY : my_sylar/log.cpp.i

my_sylar/log.s: my_sylar/log.cpp.s

.PHONY : my_sylar/log.s

# target to generate assembly for a file
my_sylar/log.cpp.s:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/log.cpp.s
.PHONY : my_sylar/log.cpp.s

my_sylar/scheduler.o: my_sylar/scheduler.cpp.o

.PHONY : my_sylar/scheduler.o

# target to build an object file
my_sylar/scheduler.cpp.o:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/scheduler.cpp.o
.PHONY : my_sylar/scheduler.cpp.o

my_sylar/scheduler.i: my_sylar/scheduler.cpp.i

.PHONY : my_sylar/scheduler.i

# target to preprocess a source file
my_sylar/scheduler.cpp.i:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/scheduler.cpp.i
.PHONY : my_sylar/scheduler.cpp.i

my_sylar/scheduler.s: my_sylar/scheduler.cpp.s

.PHONY : my_sylar/scheduler.s

# target to generate assembly for a file
my_sylar/scheduler.cpp.s:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/scheduler.cpp.s
.PHONY : my_sylar/scheduler.cpp.s

my_sylar/socket.o: my_sylar/socket.cpp.o

.PHONY : my_sylar/socket.o

# target to build an object file
my_sylar/socket.cpp.o:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/socket.cpp.o
.PHONY : my_sylar/socket.cpp.o

my_sylar/socket.i: my_sylar/socket.cpp.i

.PHONY : my_sylar/socket.i

# target to preprocess a source file
my_sylar/socket.cpp.i:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/socket.cpp.i
.PHONY : my_sylar/socket.cpp.i

my_sylar/socket.s: my_sylar/socket.cpp.s

.PHONY : my_sylar/socket.s

# target to generate assembly for a file
my_sylar/socket.cpp.s:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/socket.cpp.s
.PHONY : my_sylar/socket.cpp.s

my_sylar/thread.o: my_sylar/thread.cpp.o

.PHONY : my_sylar/thread.o

# target to build an object file
my_sylar/thread.cpp.o:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/thread.cpp.o
.PHONY : my_sylar/thread.cpp.o

my_sylar/thread.i: my_sylar/thread.cpp.i

.PHONY : my_sylar/thread.i

# target to preprocess a source file
my_sylar/thread.cpp.i:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/thread.cpp.i
.PHONY : my_sylar/thread.cpp.i

my_sylar/thread.s: my_sylar/thread.cpp.s

.PHONY : my_sylar/thread.s

# target to generate assembly for a file
my_sylar/thread.cpp.s:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/thread.cpp.s
.PHONY : my_sylar/thread.cpp.s

my_sylar/timer.o: my_sylar/timer.cpp.o

.PHONY : my_sylar/timer.o

# target to build an object file
my_sylar/timer.cpp.o:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/timer.cpp.o
.PHONY : my_sylar/timer.cpp.o

my_sylar/timer.i: my_sylar/timer.cpp.i

.PHONY : my_sylar/timer.i

# target to preprocess a source file
my_sylar/timer.cpp.i:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/timer.cpp.i
.PHONY : my_sylar/timer.cpp.i

my_sylar/timer.s: my_sylar/timer.cpp.s

.PHONY : my_sylar/timer.s

# target to generate assembly for a file
my_sylar/timer.cpp.s:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/timer.cpp.s
.PHONY : my_sylar/timer.cpp.s

my_sylar/util.o: my_sylar/util.cpp.o

.PHONY : my_sylar/util.o

# target to build an object file
my_sylar/util.cpp.o:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/util.cpp.o
.PHONY : my_sylar/util.cpp.o

my_sylar/util.i: my_sylar/util.cpp.i

.PHONY : my_sylar/util.i

# target to preprocess a source file
my_sylar/util.cpp.i:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/util.cpp.i
.PHONY : my_sylar/util.cpp.i

my_sylar/util.s: my_sylar/util.cpp.s

.PHONY : my_sylar/util.s

# target to generate assembly for a file
my_sylar/util.cpp.s:
	$(MAKE) -f CMakeFiles/my_sylar.dir/build.make CMakeFiles/my_sylar.dir/my_sylar/util.cpp.s
.PHONY : my_sylar/util.cpp.s

tests/test_address.o: tests/test_address.cc.o

.PHONY : tests/test_address.o

# target to build an object file
tests/test_address.cc.o:
	$(MAKE) -f CMakeFiles/test_address.dir/build.make CMakeFiles/test_address.dir/tests/test_address.cc.o
.PHONY : tests/test_address.cc.o

tests/test_address.i: tests/test_address.cc.i

.PHONY : tests/test_address.i

# target to preprocess a source file
tests/test_address.cc.i:
	$(MAKE) -f CMakeFiles/test_address.dir/build.make CMakeFiles/test_address.dir/tests/test_address.cc.i
.PHONY : tests/test_address.cc.i

tests/test_address.s: tests/test_address.cc.s

.PHONY : tests/test_address.s

# target to generate assembly for a file
tests/test_address.cc.s:
	$(MAKE) -f CMakeFiles/test_address.dir/build.make CMakeFiles/test_address.dir/tests/test_address.cc.s
.PHONY : tests/test_address.cc.s

tests/test_bytearray.o: tests/test_bytearray.cc.o

.PHONY : tests/test_bytearray.o

# target to build an object file
tests/test_bytearray.cc.o:
	$(MAKE) -f CMakeFiles/test_bytearray.dir/build.make CMakeFiles/test_bytearray.dir/tests/test_bytearray.cc.o
.PHONY : tests/test_bytearray.cc.o

tests/test_bytearray.i: tests/test_bytearray.cc.i

.PHONY : tests/test_bytearray.i

# target to preprocess a source file
tests/test_bytearray.cc.i:
	$(MAKE) -f CMakeFiles/test_bytearray.dir/build.make CMakeFiles/test_bytearray.dir/tests/test_bytearray.cc.i
.PHONY : tests/test_bytearray.cc.i

tests/test_bytearray.s: tests/test_bytearray.cc.s

.PHONY : tests/test_bytearray.s

# target to generate assembly for a file
tests/test_bytearray.cc.s:
	$(MAKE) -f CMakeFiles/test_bytearray.dir/build.make CMakeFiles/test_bytearray.dir/tests/test_bytearray.cc.s
.PHONY : tests/test_bytearray.cc.s

tests/test_config.o: tests/test_config.cc.o

.PHONY : tests/test_config.o

# target to build an object file
tests/test_config.cc.o:
	$(MAKE) -f CMakeFiles/test_config.dir/build.make CMakeFiles/test_config.dir/tests/test_config.cc.o
.PHONY : tests/test_config.cc.o

tests/test_config.i: tests/test_config.cc.i

.PHONY : tests/test_config.i

# target to preprocess a source file
tests/test_config.cc.i:
	$(MAKE) -f CMakeFiles/test_config.dir/build.make CMakeFiles/test_config.dir/tests/test_config.cc.i
.PHONY : tests/test_config.cc.i

tests/test_config.s: tests/test_config.cc.s

.PHONY : tests/test_config.s

# target to generate assembly for a file
tests/test_config.cc.s:
	$(MAKE) -f CMakeFiles/test_config.dir/build.make CMakeFiles/test_config.dir/tests/test_config.cc.s
.PHONY : tests/test_config.cc.s

tests/test_fiber.o: tests/test_fiber.cc.o

.PHONY : tests/test_fiber.o

# target to build an object file
tests/test_fiber.cc.o:
	$(MAKE) -f CMakeFiles/test_fiber.dir/build.make CMakeFiles/test_fiber.dir/tests/test_fiber.cc.o
.PHONY : tests/test_fiber.cc.o

tests/test_fiber.i: tests/test_fiber.cc.i

.PHONY : tests/test_fiber.i

# target to preprocess a source file
tests/test_fiber.cc.i:
	$(MAKE) -f CMakeFiles/test_fiber.dir/build.make CMakeFiles/test_fiber.dir/tests/test_fiber.cc.i
.PHONY : tests/test_fiber.cc.i

tests/test_fiber.s: tests/test_fiber.cc.s

.PHONY : tests/test_fiber.s

# target to generate assembly for a file
tests/test_fiber.cc.s:
	$(MAKE) -f CMakeFiles/test_fiber.dir/build.make CMakeFiles/test_fiber.dir/tests/test_fiber.cc.s
.PHONY : tests/test_fiber.cc.s

tests/test_hook.o: tests/test_hook.cc.o

.PHONY : tests/test_hook.o

# target to build an object file
tests/test_hook.cc.o:
	$(MAKE) -f CMakeFiles/test_hook.dir/build.make CMakeFiles/test_hook.dir/tests/test_hook.cc.o
.PHONY : tests/test_hook.cc.o

tests/test_hook.i: tests/test_hook.cc.i

.PHONY : tests/test_hook.i

# target to preprocess a source file
tests/test_hook.cc.i:
	$(MAKE) -f CMakeFiles/test_hook.dir/build.make CMakeFiles/test_hook.dir/tests/test_hook.cc.i
.PHONY : tests/test_hook.cc.i

tests/test_hook.s: tests/test_hook.cc.s

.PHONY : tests/test_hook.s

# target to generate assembly for a file
tests/test_hook.cc.s:
	$(MAKE) -f CMakeFiles/test_hook.dir/build.make CMakeFiles/test_hook.dir/tests/test_hook.cc.s
.PHONY : tests/test_hook.cc.s

tests/test_iomanager.o: tests/test_iomanager.cc.o

.PHONY : tests/test_iomanager.o

# target to build an object file
tests/test_iomanager.cc.o:
	$(MAKE) -f CMakeFiles/test_iomanager.dir/build.make CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.o
.PHONY : tests/test_iomanager.cc.o

tests/test_iomanager.i: tests/test_iomanager.cc.i

.PHONY : tests/test_iomanager.i

# target to preprocess a source file
tests/test_iomanager.cc.i:
	$(MAKE) -f CMakeFiles/test_iomanager.dir/build.make CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.i
.PHONY : tests/test_iomanager.cc.i

tests/test_iomanager.s: tests/test_iomanager.cc.s

.PHONY : tests/test_iomanager.s

# target to generate assembly for a file
tests/test_iomanager.cc.s:
	$(MAKE) -f CMakeFiles/test_iomanager.dir/build.make CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.s
.PHONY : tests/test_iomanager.cc.s

tests/test_log.o: tests/test_log.cc.o

.PHONY : tests/test_log.o

# target to build an object file
tests/test_log.cc.o:
	$(MAKE) -f CMakeFiles/test_log.dir/build.make CMakeFiles/test_log.dir/tests/test_log.cc.o
.PHONY : tests/test_log.cc.o

tests/test_log.i: tests/test_log.cc.i

.PHONY : tests/test_log.i

# target to preprocess a source file
tests/test_log.cc.i:
	$(MAKE) -f CMakeFiles/test_log.dir/build.make CMakeFiles/test_log.dir/tests/test_log.cc.i
.PHONY : tests/test_log.cc.i

tests/test_log.s: tests/test_log.cc.s

.PHONY : tests/test_log.s

# target to generate assembly for a file
tests/test_log.cc.s:
	$(MAKE) -f CMakeFiles/test_log.dir/build.make CMakeFiles/test_log.dir/tests/test_log.cc.s
.PHONY : tests/test_log.cc.s

tests/test_scheduler.o: tests/test_scheduler.cc.o

.PHONY : tests/test_scheduler.o

# target to build an object file
tests/test_scheduler.cc.o:
	$(MAKE) -f CMakeFiles/test_scheduler.dir/build.make CMakeFiles/test_scheduler.dir/tests/test_scheduler.cc.o
.PHONY : tests/test_scheduler.cc.o

tests/test_scheduler.i: tests/test_scheduler.cc.i

.PHONY : tests/test_scheduler.i

# target to preprocess a source file
tests/test_scheduler.cc.i:
	$(MAKE) -f CMakeFiles/test_scheduler.dir/build.make CMakeFiles/test_scheduler.dir/tests/test_scheduler.cc.i
.PHONY : tests/test_scheduler.cc.i

tests/test_scheduler.s: tests/test_scheduler.cc.s

.PHONY : tests/test_scheduler.s

# target to generate assembly for a file
tests/test_scheduler.cc.s:
	$(MAKE) -f CMakeFiles/test_scheduler.dir/build.make CMakeFiles/test_scheduler.dir/tests/test_scheduler.cc.s
.PHONY : tests/test_scheduler.cc.s

tests/test_socket.o: tests/test_socket.cc.o

.PHONY : tests/test_socket.o

# target to build an object file
tests/test_socket.cc.o:
	$(MAKE) -f CMakeFiles/test_socket.dir/build.make CMakeFiles/test_socket.dir/tests/test_socket.cc.o
.PHONY : tests/test_socket.cc.o

tests/test_socket.i: tests/test_socket.cc.i

.PHONY : tests/test_socket.i

# target to preprocess a source file
tests/test_socket.cc.i:
	$(MAKE) -f CMakeFiles/test_socket.dir/build.make CMakeFiles/test_socket.dir/tests/test_socket.cc.i
.PHONY : tests/test_socket.cc.i

tests/test_socket.s: tests/test_socket.cc.s

.PHONY : tests/test_socket.s

# target to generate assembly for a file
tests/test_socket.cc.s:
	$(MAKE) -f CMakeFiles/test_socket.dir/build.make CMakeFiles/test_socket.dir/tests/test_socket.cc.s
.PHONY : tests/test_socket.cc.s

tests/test_thread.o: tests/test_thread.cc.o

.PHONY : tests/test_thread.o

# target to build an object file
tests/test_thread.cc.o:
	$(MAKE) -f CMakeFiles/test_thread.dir/build.make CMakeFiles/test_thread.dir/tests/test_thread.cc.o
.PHONY : tests/test_thread.cc.o

tests/test_thread.i: tests/test_thread.cc.i

.PHONY : tests/test_thread.i

# target to preprocess a source file
tests/test_thread.cc.i:
	$(MAKE) -f CMakeFiles/test_thread.dir/build.make CMakeFiles/test_thread.dir/tests/test_thread.cc.i
.PHONY : tests/test_thread.cc.i

tests/test_thread.s: tests/test_thread.cc.s

.PHONY : tests/test_thread.s

# target to generate assembly for a file
tests/test_thread.cc.s:
	$(MAKE) -f CMakeFiles/test_thread.dir/build.make CMakeFiles/test_thread.dir/tests/test_thread.cc.s
.PHONY : tests/test_thread.cc.s

tests/test_util.o: tests/test_util.cc.o

.PHONY : tests/test_util.o

# target to build an object file
tests/test_util.cc.o:
	$(MAKE) -f CMakeFiles/test_util.dir/build.make CMakeFiles/test_util.dir/tests/test_util.cc.o
.PHONY : tests/test_util.cc.o

tests/test_util.i: tests/test_util.cc.i

.PHONY : tests/test_util.i

# target to preprocess a source file
tests/test_util.cc.i:
	$(MAKE) -f CMakeFiles/test_util.dir/build.make CMakeFiles/test_util.dir/tests/test_util.cc.i
.PHONY : tests/test_util.cc.i

tests/test_util.s: tests/test_util.cc.s

.PHONY : tests/test_util.s

# target to generate assembly for a file
tests/test_util.cc.s:
	$(MAKE) -f CMakeFiles/test_util.dir/build.make CMakeFiles/test_util.dir/tests/test_util.cc.s
.PHONY : tests/test_util.cc.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... test_scheduler"
	@echo "... test_config"
	@echo "... my_sylar"
	@echo "... test_bytearray"
	@echo "... test_thread"
	@echo "... rebuild_cache"
	@echo "... edit_cache"
	@echo "... test_util"
	@echo "... test_fiber"
	@echo "... test_log"
	@echo "... test_address"
	@echo "... test_iomanager"
	@echo "... test_hook"
	@echo "... test_socket"
	@echo "... my_sylar/address.o"
	@echo "... my_sylar/address.i"
	@echo "... my_sylar/address.s"
	@echo "... my_sylar/bytearray.o"
	@echo "... my_sylar/bytearray.i"
	@echo "... my_sylar/bytearray.s"
	@echo "... my_sylar/config.o"
	@echo "... my_sylar/config.i"
	@echo "... my_sylar/config.s"
	@echo "... my_sylar/fd_manager.o"
	@echo "... my_sylar/fd_manager.i"
	@echo "... my_sylar/fd_manager.s"
	@echo "... my_sylar/fiber.o"
	@echo "... my_sylar/fiber.i"
	@echo "... my_sylar/fiber.s"
	@echo "... my_sylar/hook.o"
	@echo "... my_sylar/hook.i"
	@echo "... my_sylar/hook.s"
	@echo "... my_sylar/iomanager.o"
	@echo "... my_sylar/iomanager.i"
	@echo "... my_sylar/iomanager.s"
	@echo "... my_sylar/log.o"
	@echo "... my_sylar/log.i"
	@echo "... my_sylar/log.s"
	@echo "... my_sylar/scheduler.o"
	@echo "... my_sylar/scheduler.i"
	@echo "... my_sylar/scheduler.s"
	@echo "... my_sylar/socket.o"
	@echo "... my_sylar/socket.i"
	@echo "... my_sylar/socket.s"
	@echo "... my_sylar/thread.o"
	@echo "... my_sylar/thread.i"
	@echo "... my_sylar/thread.s"
	@echo "... my_sylar/timer.o"
	@echo "... my_sylar/timer.i"
	@echo "... my_sylar/timer.s"
	@echo "... my_sylar/util.o"
	@echo "... my_sylar/util.i"
	@echo "... my_sylar/util.s"
	@echo "... tests/test_address.o"
	@echo "... tests/test_address.i"
	@echo "... tests/test_address.s"
	@echo "... tests/test_bytearray.o"
	@echo "... tests/test_bytearray.i"
	@echo "... tests/test_bytearray.s"
	@echo "... tests/test_config.o"
	@echo "... tests/test_config.i"
	@echo "... tests/test_config.s"
	@echo "... tests/test_fiber.o"
	@echo "... tests/test_fiber.i"
	@echo "... tests/test_fiber.s"
	@echo "... tests/test_hook.o"
	@echo "... tests/test_hook.i"
	@echo "... tests/test_hook.s"
	@echo "... tests/test_iomanager.o"
	@echo "... tests/test_iomanager.i"
	@echo "... tests/test_iomanager.s"
	@echo "... tests/test_log.o"
	@echo "... tests/test_log.i"
	@echo "... tests/test_log.s"
	@echo "... tests/test_scheduler.o"
	@echo "... tests/test_scheduler.i"
	@echo "... tests/test_scheduler.s"
	@echo "... tests/test_socket.o"
	@echo "... tests/test_socket.i"
	@echo "... tests/test_socket.s"
	@echo "... tests/test_thread.o"
	@echo "... tests/test_thread.i"
	@echo "... tests/test_thread.s"
	@echo "... tests/test_util.o"
	@echo "... tests/test_util.i"
	@echo "... tests/test_util.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

