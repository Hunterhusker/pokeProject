cmake_minimum_required(VERSION 3.21)
project(pokeProject)

set(CMAKE_CXX_STANDARD 14)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(pokeProject main.cpp minHeap.cpp mapBuilder.cpp heatMap.cpp entity.cpp pokemon.cpp fileReader.cpp menus.cpp world.cpp)
target_link_libraries(pokeProject ${CURSES_LIBRARIES})