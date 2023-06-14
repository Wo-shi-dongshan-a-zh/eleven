cmake_minimum_required(VERSION 2.8)
project(eleven)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

set (LIBSRC
    ./eleven/log/log.cpp
    ./eleven/util/util.cpp
)
add_library(eleven SHARED ${LIBSRC})

add_executable(test test/test.cpp)
add_dependencies(test eleven)
target_link_libraries(test eleven)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
