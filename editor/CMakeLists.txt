file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(OrigoEditor
    ${EDITOR_SOURCES}
    ${CMAKE_SOURCE_DIR}/vendor/imguizmo/ImGuizmo.cpp
)

target_include_directories(OrigoEditor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendor/nanosvg/src
    ${CMAKE_SOURCE_DIR}/vendor/imguizmo
)


target_link_libraries(OrigoEditor PRIVATE
    OrigoEngine
    imgui
)

target_precompile_headers(OrigoEditor PRIVATE
    ${CMAKE_SOURCE_DIR}/pch.h
)

add_custom_command(TARGET OrigoEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:OrigoEngine>
        $<TARGET_FILE_DIR:OrigoEditor>
)
