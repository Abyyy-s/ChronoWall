cmake_minimum_required(VERSION 3.16)

project(ChronoWall)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(ChronoWall
    src/main.cpp
    src/WallpaperFrame.cpp
    src/WallpaperParser.cpp
)
