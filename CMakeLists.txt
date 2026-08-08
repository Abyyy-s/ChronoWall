cmake_minimum_required(VERSION 3.16)

project(ChronoWall)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(tinyxml2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)

add_executable(ChronoWall
    src/main.cpp
    src/WallpaperFrame.cpp
    src/WallpaperParser.cpp
    src/WallpaperScheduler.cpp
    src/WallpaperChanger.cpp
    src/DynamicWallpaper.cpp
    src/Transition.cpp
    src/TimelineEvent.cpp
    src/TransitionRenderer.cpp
    src/X11DesktopSurface.cpp
)

target_include_directories(ChronoWall PRIVATE
    include
    ${SDL3_INCLUDE_DIRS}
)

target_link_directories(ChronoWall PRIVATE ${SDL3_LIBRARY_DIRS})

target_link_libraries(ChronoWall PRIVATE
    tinyxml2::tinyxml2
    ${SDL3_LIBRARIES}
)
