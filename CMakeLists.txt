cmake_minimum_required(VERSION 3.21)
project(pokeProject C)

set(CMAKE_C_STANDARD 11)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(pokeProject main.c minHeap.c mapBuilder.c heatMap.c entity.c)
target_link_libraries(pokeProject ${CURSES_LIBRARIES})