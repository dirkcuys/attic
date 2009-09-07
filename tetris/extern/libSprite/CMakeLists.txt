cmake_minimum_required(VERSION 2.6)

set(include_paths /usr/include/SDL)
find_library(SDL names sdl)
include_directories( ${include_paths} )

add_library(libSprite libSprite.c Sprite.h)
target_link_libraries(libSprite SDL)
