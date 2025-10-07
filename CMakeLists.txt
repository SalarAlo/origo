cmake_minimum_required(VERSION 3.16)
project(Origo LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output dirs
set(output_dir ${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${output_dir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_OBJECT_PATH_MAX 512)

# Dependencies
find_package(Threads REQUIRED)

# Vendor libs
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)

set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/assimp)

add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

# Add engine + game
add_subdirectory(origo)
add_subdirectory(game)
