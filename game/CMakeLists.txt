project(OrigoGame LANGUAGES CXX)

file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(OrigoGame ${GAME_SOURCES})

target_link_libraries(OrigoGame PRIVATE OrigoEngine)

target_precompile_headers(OrigoGame PRIVATE ${CMAKE_SOURCE_DIR}/pch.h)

# Copy resources next to the binary
add_custom_command(TARGET OrigoGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:OrigoEngine>
        $<TARGET_FILE_DIR:OrigoGame>
)

