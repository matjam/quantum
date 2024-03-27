cmake_minimum_required(VERSION 3.28)

file(STRINGS "version.txt" version REGEX "^([0-9]+)\\.([0-9]+)\\.([0-9]+)")

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)

project(quantum
    VERSION ${version}
    DESCRIPTION "A 2d Rogue-like game"
    LANGUAGES CXX
)

configure_file(src/Version.hpp.in src/Version.hpp)

string(TOLOWER ${CMAKE_BUILD_TYPE} build_type)
if (build_type STREQUAL "debug")
    add_compile_options(-O0 -g -Wall -Wextra -Wpedantic -flto)
else()
    add_compile_options(-O2 -Wall -Wextra -Wpedantic -Werror)
endif()

# ---- libraries ----
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(SFML COMPONENTS system window graphics audio CONFIG REQUIRED)

# ---- target ----
add_executable(
    ${CMAKE_PROJECT_NAME}
    src/main.cpp
    src/Tilesheet.cpp
    src/Tilemap.cpp
    src/RoomShape.cpp
    src/Maze.cpp
    src/TilesheetExplorer.cpp
    src/Autotile.cpp
)

target_compile_features(quantum PRIVATE cxx_std_20)
target_link_libraries(quantum PRIVATE 
    fmt::fmt 
    spdlog::spdlog 
    sfml-system sfml-graphics sfml-window sfml-audio
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}/src")

