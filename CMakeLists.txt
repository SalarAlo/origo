cmake_minimum_required(VERSION 3.16)
project(${PROJECT_NAME} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories

set(output_dir ${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${output_dir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_OBJECT_PATH_MAX 512)

# Dependencies

find_package(Threads REQUIRED)   # for Threads::Threads

# GLFW (already has its own CMakeLists.txt)
add_subdirectory(vendor/glfw)

# GLM (header-only but has its own CMakeLists)
add_subdirectory(vendor/glm)

# Assimp (big build system, so tweak a few options)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/assimp)

# GLAD (manual)
add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

# NonEuclidianEngine project

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/doompch.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    include
    vendor/glfw/include
    vendor/glad/include
    vendor/glm
    vendor/assimp/include
    vendor/stb
)

# Precompiled header (modern CMake way)
target_precompile_headers(${PROJECT_NAME} PRIVATE doompch.h)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glad
    glm
    assimp
    Threads::Threads
)

# Resource copying (so shaders are always next to the binary)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)
