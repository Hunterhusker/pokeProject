cmake_minimum_required(VERSION 3.21)
project(pokeProject C)

set(CMAKE_C_STANDARD 11)

add_executable(pokeProject main.c minHeap.c mapBuilder.c heatMap.c player.c entity.c)
