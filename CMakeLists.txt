cmake_minimum_required(VERSION 3.31)
project(snake C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_FLAGS "-lncurses")

add_executable(snake main.c)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
target_link_libraries(snake ${CURSES_LIBRARIES})