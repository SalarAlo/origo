cmake_minimum_required(VERSION 3.16)
project(Origo LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------------------------
# Global configuration
# ------------------------------------------------------------------------------

# Ensure all targets (even static libs) are compiled with -fPIC
# This is essential when linking static libs into shared libs (.so)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")

# Output directories
set(output_dir ${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${output_dir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_OBJECT_PATH_MAX 512)

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(UUID REQUIRED uuid)

# ------------------------------------------------------------------------------
# Vendor Libraries
# ------------------------------------------------------------------------------

# GLFW / GLM
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)

# ASSIMP
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

find_package(ZLIB REQUIRED)
add_subdirectory(vendor/assimp)

# GLAD (static but PIC-safe)
add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

# nanosvg
include_directories(vendor/nanosvg/src)

# Ensure glad uses -fPIC if ever linked into shared libs
set_target_properties(glad PROPERTIES POSITION_INDEPENDENT_CODE ON)

# MAGIC_ENUM
include_directories(vendor/magic_enum/include)

# IMGUI
add_subdirectory(vendor/imgui)

# SPDLOG (header-only)
set(SPDLOG_HEADER_ONLY ON)
add_subdirectory(vendor/spdlog)

# NLOHMANN/JSON
add_subdirectory(vendor/json)

# ------------------------------------------------------------------------------
# Engine, Game, Editor
# ------------------------------------------------------------------------------

add_subdirectory(origo)
add_subdirectory(game)
add_subdirectory(editor)
