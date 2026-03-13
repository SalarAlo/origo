cmake_minimum_required(VERSION 3.16)

file(GLOB_RECURSE JOLT_SOURCES
    Jolt/*.cpp
)

add_library(jolt STATIC ${JOLT_SOURCES})

target_include_directories(jolt PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(jolt PUBLIC cxx_std_20)

target_compile_definitions(jolt PUBLIC
    JPH_PROFILE_ENABLED
    JPH_DEBUG_RENDERER
    JPH_FLOATING_POINT_EXCEPTIONS_ENABLED=0
)

target_link_libraries(jolt PUBLIC
    Threads::Threads
)

set_target_properties(jolt PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
