cmake_minimum_required(VERSION 3.22)
project(xcontiki_exe C)

set(CMAKE_C_STANDARD 99)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/arch/template)

add_executable(xcontiki_exe main.c)

add_subdirectory(xcontiki)
add_subdirectory(arch/template/xcontiki/arch)

target_link_libraries(xcontiki_exe xcontiki)

