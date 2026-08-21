cmake_minimum_required(VERSION 3.20)
project(game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# Dont build raylibs ~150 examples programs - they tripe the build time.
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAB 5.5
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(raylib)
add_executable(game src/main.cpp)
target_link_libraries(game PRIVATE raylib)