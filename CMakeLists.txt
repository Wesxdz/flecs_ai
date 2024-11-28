cmake_minimum_required(VERSION 3.16)
project(quest)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")
# find_package(SDL2 REQUIRED)
# find_package(SDL2_image REQUIRED)

# Build the shared library for vital_module, explicitly as C
add_library(vital_module SHARED)
set_target_properties(vital_module PROPERTIES LANGUAGE C)
target_sources(vital_module PRIVATE src/vital_module.c)
set_target_properties(vital_module PROPERTIES OUTPUT_NAME "vital_module")

# Include directories
include_directories("src")
include_directories("deps/flecs-4.0.3/include")
include_directories("deps/json-3.11.3/include")
# include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

# Add the source files for the main executable
# add_executable(quest src/main.cpp)
add_executable(quest src/main.cpp)

# Add dependencies
add_subdirectory("deps/flecs-4.0.3")

# Link vital_module and other libraries
# target_link_libraries(quest ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} flecs vital_module)
target_link_libraries(quest flecs vital_module)
