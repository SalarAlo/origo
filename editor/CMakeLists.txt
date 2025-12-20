project(OrigoEditorApp LANGUAGES C CXX)

# ----------------------------------------------------------------------
# Sources
# ----------------------------------------------------------------------

file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(OrigoEditor
    ${EDITOR_SOURCES}
    ${CMAKE_SOURCE_DIR}/vendor/imguizmo/ImGuizmo.cpp
)

# ----------------------------------------------------------------------
# Includes
# ----------------------------------------------------------------------

target_include_directories(OrigoEditor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendor/nanosvg/src
    ${CMAKE_SOURCE_DIR}/vendor/imguizmo
)

# ----------------------------------------------------------------------
# Linkage
# ----------------------------------------------------------------------

target_link_libraries(OrigoEditor PRIVATE
    OrigoEngine
    imgui
)

# ----------------------------------------------------------------------
# PCH
# ----------------------------------------------------------------------

target_precompile_headers(OrigoEditor PRIVATE
    ${CMAKE_SOURCE_DIR}/pch.h
)

# ----------------------------------------------------------------------
# Runtime copy of engine .so
# ----------------------------------------------------------------------

add_custom_command(TARGET OrigoEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:OrigoEngine>
        $<TARGET_FILE_DIR:OrigoEditor>
)
