cmake_minimum_required(VERSION 3.16)
project(FlecsSDL3Example)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

# Add the source files
add_executable(quest src/main.cpp)

add_subdirectory("deps/flecs-4.0.3")

include_directories("src")
include_directories("deps/flecs-4.0.3/include")
include_directories("deps/json-3.11.3/include")

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

# Link SDL3 and SDL3_image
target_link_libraries(quest ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} flecs)
