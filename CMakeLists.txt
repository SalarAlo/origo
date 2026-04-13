# root CMakeLists.txt

cmake_minimum_required(VERSION 3.16)
cmake_policy(VERSION 3.16)

project(Origo LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")

set(output_dir ${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(ORIGO_ENABLE_CLANG_TIDY "Run clang-tidy during builds for project C++ targets" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${output_dir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${output_dir}")

set(BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_QT OFF CACHE BOOL "" FORCE)
set(BUILD_IMGUI ON CACHE BOOL "" FORCE) 

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(UUID REQUIRED uuid)
pkg_check_modules(LUA54 REQUIRED IMPORTED_TARGET lua5.4)
find_package(ZLIB REQUIRED)


# ------------------------------------------------------------------------------
# Vendor Libraries
# ------------------------------------------------------------------------------

function(origo_require_submodule dir)
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/${dir}/CMakeLists.txt")
        message(FATAL_ERROR
            "Missing vendored dependency '${dir}'. "
            "Ensure the full source tree is present in the repository checkout."
        )
    endif()
endfunction()

origo_require_submodule(vendor/glfw)
origo_require_submodule(vendor/glm)
origo_require_submodule(vendor/assimp)
origo_require_submodule(vendor/imgui)
origo_require_submodule(vendor/spdlog)
origo_require_submodule(vendor/json)
origo_require_submodule(vendor/jolt)
origo_require_submodule(vendor/nativefiledialog)
origo_require_submodule(vendor/zep)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/assimp)
add_subdirectory(vendor/imgui)
add_library(imgui_textedit
    vendor/ImGuiColorTextEdit/TextEditor.cpp
)
target_include_directories(imgui_textedit PUBLIC
    vendor/ImGuiColorTextEdit
)
target_link_libraries(imgui_textedit PUBLIC imgui)

set(ZEP_ROOT "${CMAKE_SOURCE_DIR}/vendor/zep")
configure_file(
    "${ZEP_ROOT}/cmake/config_app.h.cmake"
    "${CMAKE_BINARY_DIR}/config_app.h"
)

add_library(zep_vendor STATIC
    vendor/zep/src/buffer.cpp
    vendor/zep/src/commands.cpp
    vendor/zep/src/display.cpp
    vendor/zep/src/editor.cpp
    vendor/zep/src/filesystem.cpp
    vendor/zep/src/glyph_iterator.cpp
    vendor/zep/src/indexer.cpp
    vendor/zep/src/keymap.cpp
    vendor/zep/src/line_widgets.cpp
    vendor/zep/src/mcommon/animation/timer.cpp
    vendor/zep/src/mcommon/file/path.cpp
    vendor/zep/src/mcommon/string/stringutils.cpp
    vendor/zep/src/mode.cpp
    vendor/zep/src/mode_repl.cpp
    vendor/zep/src/mode_search.cpp
    vendor/zep/src/mode_standard.cpp
    vendor/zep/src/mode_tree.cpp
    vendor/zep/src/mode_vim.cpp
    vendor/zep/src/range_markers.cpp
    vendor/zep/src/regress.cpp
    vendor/zep/src/scroller.cpp
    vendor/zep/src/splits.cpp
    vendor/zep/src/syntax.cpp
    vendor/zep/src/syntax_markdown.cpp
    vendor/zep/src/syntax_providers.cpp
    vendor/zep/src/syntax_rainbow_brackets.cpp
    vendor/zep/src/syntax_tree.cpp
    vendor/zep/src/tab_window.cpp
    vendor/zep/src/theme.cpp
    vendor/zep/src/window.cpp
)
target_include_directories(zep_vendor PUBLIC
    vendor/zep/include
    vendor/zep/src
    vendor/zep/src/mcommon
    ${CMAKE_BINARY_DIR}
)
target_compile_definitions(zep_vendor PUBLIC
    ZEP_FEATURE_CPP_FILE_SYSTEM
)
target_link_libraries(zep_vendor PUBLIC imgui)
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
target_compile_definitions(sol2 INTERFACE
    SOL_NO_LUA_HPP=1
)
target_link_libraries(sol2 INTERFACE PkgConfig::LUA54)

include_directories(
    vendor/nanosvg/src
    vendor/magic_enum/include
)

# ------------------------------------------------------------------------------
# Testing
# ------------------------------------------------------------------------------

option(ORIGO_ENABLE_TESTS "Build Origo tests" ON)

if (ORIGO_ENABLE_TESTS)
    enable_testing()
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

# ------------------------------------------------------------------------------
# clang-tidy
# ------------------------------------------------------------------------------

function(origo_collect_targets dir out_var)
    get_property(local_targets DIRECTORY "${dir}" PROPERTY BUILDSYSTEM_TARGETS)
    set(all_targets ${local_targets})

    get_property(subdirs DIRECTORY "${dir}" PROPERTY SUBDIRECTORIES)
    foreach(subdir IN LISTS subdirs)
        origo_collect_targets("${subdir}" nested_targets)
        list(APPEND all_targets ${nested_targets})
    endforeach()

    set(${out_var} "${all_targets}" PARENT_SCOPE)
endfunction()

function(origo_enable_clang_tidy)
    if(NOT ORIGO_ENABLE_CLANG_TIDY)
        return()
    endif()

    find_program(ORIGO_CLANG_TIDY_EXE NAMES clang-tidy)
    if(NOT ORIGO_CLANG_TIDY_EXE)
        message(STATUS "clang-tidy not found; skipping build-time lint integration")
        return()
    endif()

    file(TO_CMAKE_PATH "${CMAKE_SOURCE_DIR}" origo_source_root)
    string(REGEX REPLACE "([][+.*^$(){}|\\\\])" "\\\\\\1" origo_source_root_regex "${origo_source_root}")
    set(origo_header_filter
        "^(${origo_source_root_regex}/(origo|editor|tests)(/|$)|${origo_source_root_regex}/pch\\.h$)"
    )
    set(origo_clang_tidy_command
        "${ORIGO_CLANG_TIDY_EXE}"
        "-p=${CMAKE_BINARY_DIR}"
        "--header-filter=${origo_header_filter}"
    )

    origo_collect_targets("${CMAKE_SOURCE_DIR}" all_targets)

    foreach(target_name IN LISTS all_targets)
        get_target_property(target_type "${target_name}" TYPE)
        if(target_type STREQUAL "INTERFACE_LIBRARY" OR target_type STREQUAL "UTILITY")
            continue()
        endif()

        get_target_property(target_sources "${target_name}" SOURCES)
        if(NOT target_sources)
            continue()
        endif()

        get_target_property(target_source_dir "${target_name}" SOURCE_DIR)
        set(has_project_cpp_source FALSE)
        set(has_vendor_cpp_source FALSE)

        foreach(target_source IN LISTS target_sources)
            if(target_source MATCHES "^\\$<")
                continue()
            endif()

            if(IS_ABSOLUTE "${target_source}")
                set(source_abs "${target_source}")
            else()
                get_filename_component(source_abs "${target_source}" ABSOLUTE BASE_DIR "${target_source_dir}")
            endif()

            file(TO_CMAKE_PATH "${source_abs}" source_abs_normalized)
            if(NOT source_abs_normalized MATCHES "\\.(cc|cp|cxx|cpp|CPP)$")
                continue()
            endif()

            if(source_abs_normalized MATCHES "^${origo_source_root_regex}/vendor(/|$)")
                set(has_vendor_cpp_source TRUE)
            elseif(source_abs_normalized MATCHES "^${origo_source_root_regex}(/|$)")
                set(has_project_cpp_source TRUE)
            endif()
        endforeach()

        if(has_project_cpp_source AND NOT has_vendor_cpp_source)
            set_target_properties("${target_name}" PROPERTIES CXX_CLANG_TIDY "${origo_clang_tidy_command}")
        endif()
    endforeach()
endfunction()

origo_enable_clang_tidy()
