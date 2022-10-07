cmake_minimum_required(VERSION 3.22)
project(xcontiki_exe C)

set(CMAKE_C_STANDARD 99)

add_executable(xcontiki_exe main.c)

add_subdirectory(xcontiki)

target_link_libraries(xcontiki_exe xcontiki)

