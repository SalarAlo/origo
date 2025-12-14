project(OrigoEngine LANGUAGES CXX)

# Engine source files
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/pch.cpp
)

# Create engine as a shared library
add_library(OrigoEngine SHARED ${ENGINE_SOURCES})

# Public includes means: visible to games using the engine
target_include_directories(OrigoEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendor/glfw/include
    ${CMAKE_SOURCE_DIR}/vendor/glad/include
    ${CMAKE_SOURCE_DIR}/vendor/glm
    ${CMAKE_SOURCE_DIR}/vendor/assimp/include
    ${CMAKE_SOURCE_DIR}/vendor/stb
    ${CMAKE_SOURCE_DIR}/vendor/imgui
    ${CMAKE_SOURCE_DIR}/vendor/imgui/backends
)

# Link engine dependencies
target_link_libraries(OrigoEngine PUBLIC
    glfw
    glad
    glm
    assimp
    imgui  # ImGui now belongs to the engine
    Threads::Threads
    spdlog_header_only
    nlohmann_json::nlohmann_json
    ankerl_unordered_dense

    ZLIB::ZLIB

    X11
    Xrandr
    Xi
    Xxf86vm
    Xcursor
    GL
)

# Expose engine defines for DLL build
target_compile_definitions(OrigoEngine PRIVATE ORIGO_BUILD_DLL)

# Precompiled headers for faster builds
target_precompile_headers(OrigoEngine PRIVATE ${CMAKE_SOURCE_DIR}/pch.h)
