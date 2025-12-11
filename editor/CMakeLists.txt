project(OrigoEditorApp LANGUAGES C CXX)

file(GLOB_RECURSE  EDITOR_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(OrigoEditor ${EDITOR_SOURCES})

target_link_libraries(OrigoEditor PRIVATE OrigoEngine)
target_include_directories(OrigoEditor  PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/vendor/nanosvg/src
)

target_precompile_headers(OrigoEditor PRIVATE ${CMAKE_SOURCE_DIR}/pch.h)

add_custom_command(TARGET OrigoEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:OrigoEngine>
        $<TARGET_FILE_DIR:OrigoEditor>
)
