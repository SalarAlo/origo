# editor/CMakeLists.txt

file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS
				${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/src/*/*/*.cpp
)

add_library(imguizmo_vendor STATIC
				${CMAKE_SOURCE_DIR}/vendor/imguizmo/ImGuizmo.cpp
)

target_include_directories(imguizmo_vendor PUBLIC
				${CMAKE_SOURCE_DIR}/vendor/imguizmo
)
target_link_libraries(imguizmo_vendor PUBLIC imgui)

add_executable(OrigoEditor
				${EDITOR_SOURCES}
)

target_include_directories(OrigoEditor PRIVATE
				${CMAKE_CURRENT_SOURCE_DIR}/src
				${CMAKE_SOURCE_DIR}/vendor/nanosvg/src
				${CMAKE_SOURCE_DIR}/vendor/imguizmo
)

target_link_libraries(OrigoEditor PRIVATE
				OrigoEngine
				nfd
				imgui
				imgui_textedit
				imguizmo_vendor
)

target_precompile_headers(OrigoEditor PRIVATE
				${CMAKE_SOURCE_DIR}/pch.h
)

add_custom_command(TARGET OrigoEditor POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy
								$<TARGET_FILE:OrigoEngine>
								$<TARGET_FILE_DIR:OrigoEditor>
)
