cmake_minimum_required(VERSION 2.6)

set(include_paths /usr/include/SDL)
find_library(SDL names sdl)
include_directories( ${include_paths} )

add_library(libFont libFont.c Font.h)
target_link_libraries(libFont SDL)
