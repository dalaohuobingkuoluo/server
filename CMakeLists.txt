cmake_minimum_required(VERSION 2.8)
project(my_sylar)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated  -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
# -Werror
set(LIB_SRC 
    my_sylar/log.cpp
    my_sylar/util.cpp
    my_sylar/config.cpp
    my_sylar/thread.cpp
    my_sylar/fiber.cpp
    my_sylar/scheduler.cpp
    my_sylar/iomanager.cpp
    my_sylar/timer.cpp
    my_sylar/hook.cpp
    my_sylar/fd_manager.cpp
    my_sylar/address.cpp
    my_sylar/socket.cpp
    my_sylar/bytearray.cpp
    )

add_library(my_sylar SHARED ${LIB_SRC})

set(LIBS
    my_sylar
    dl
    yaml-cpp 
    pthread)

add_executable(test_log tests/test_log.cc)
#add_dependencies(test_log my_sylar)
target_link_libraries(test_log ${LIBS})

add_executable(test_config tests/test_config.cc)
#add_dependencies(test_config my_sylar)
target_link_libraries(test_config ${LIBS})

add_executable(test_thread tests/test_thread.cc)
#add_dependencies(test_thread my_sylar)
target_link_libraries(test_thread ${LIBS})

add_executable(test_util tests/test_util.cc)
#add_dependencies(test_thread my_sylar)
target_link_libraries(test_util ${LIBS})

add_executable(test_fiber tests/test_fiber.cc)
#add_dependencies(test_thread my_sylar)
target_link_libraries(test_fiber ${LIBS})

add_executable(test_scheduler tests/test_scheduler.cc)
#add_dependencies(test_thread my_sylar)
target_link_libraries(test_scheduler ${LIBS})

add_executable(test_iomanager tests/test_iomanager.cc)
#add_dependencies(test_thread my_sylar)
target_link_libraries(test_iomanager ${LIBS})

add_executable(test_hook tests/test_hook.cc)
#add_dependencies(test_thread my_sylar)
target_link_libraries(test_hook ${LIBS})

add_executable(test_address tests/test_address.cc)
#add_dependencies(test_thread my_sylar)
target_link_libraries(test_address ${LIBS})

add_executable(test_socket tests/test_socket.cc)
#add_dependencies(test_thread my_sylar)
target_link_libraries(test_socket ${LIBS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
