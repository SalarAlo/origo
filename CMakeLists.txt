# root CMakeLists.txt

cmake_minimum_required(VERSION 3.16)
cmake_policy(VERSION 3.16)

project(Origo LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")

set(output_dir ${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${output_dir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(UUID REQUIRED uuid)
find_package(ZLIB REQUIRED)


# ------------------------------------------------------------------------------
# Vendor Libraries
# ------------------------------------------------------------------------------

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/assimp)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/json)
add_subdirectory(vendor/jolt)
add_subdirectory(vendor/nativefiledialog)

add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)
set_target_properties(glad PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(ankerl_unordered_dense INTERFACE)
target_include_directories(ankerl_unordered_dense INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/ankerl/include
)

add_library(sol2 INTERFACE)
target_include_directories(sol2 INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/sol2/include
)

include_directories(
    vendor/nanosvg/src
    vendor/magic_enum/include
)

# ------------------------------------------------------------------------------
# Testing
# ------------------------------------------------------------------------------

option(ORIGO_ENABLE_TESTS "Build Origo tests" ON)

if (ORIGO_ENABLE_TESTS)
    include(FetchContent)

    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.11
    )

    FetchContent_Populate(doctest)

    add_library(doctest INTERFACE)
    target_include_directories(doctest INTERFACE
        ${doctest_SOURCE_DIR}
    )

    add_library(doctest::doctest ALIAS doctest)

    add_subdirectory(tests)
endif()


# ------------------------------------------------------------------------------
# Engine, Editor
# ------------------------------------------------------------------------------

add_subdirectory(origo)
add_subdirectory(editor)
