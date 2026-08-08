cmake_minimum_required(VERSION 3.16)

project(ChronoWall)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(tinyxml2 REQUIRED)

add_executable(ChronoWall
    src/main.cpp
    src/WallpaperFrame.cpp
    src/WallpaperParser.cpp
    src/WallpaperScheduler.cpp
    src/WallpaperChanger.cpp
    src/DynamicWallpaper.cpp
    src/Transition.cpp
    src/TimelineEvent.cpp
)

target_include_directories(ChronoWall PRIVATE
    include
)

target_link_libraries(ChronoWall PRIVATE
    tinyxml2::tinyxml2
)
