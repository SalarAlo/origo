project(OrigoEngine LANGUAGES CXX)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/pch.cpp   # if you want PCH inside engine
)

add_library(OrigoEngine STATIC ${ENGINE_SOURCES})

target_include_directories(OrigoEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendor/glfw/include
    ${CMAKE_SOURCE_DIR}/vendor/glad/include
    ${CMAKE_SOURCE_DIR}/vendor/glm
    ${CMAKE_SOURCE_DIR}/vendor/assimp/include
    ${CMAKE_SOURCE_DIR}/vendor/stb
    ${CMAKE_SOURCE_DIR}/vendor/glfw

)

target_link_libraries(OrigoEngine PUBLIC
    glfw
    glad
    glm
    assimp
    Threads::Threads
    spdlog_header_only
)

target_precompile_headers(OrigoEngine PRIVATE ${CMAKE_SOURCE_DIR}/pch.h)
