# origo/CMakeLists.txt

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
				${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/*/*/*.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/*/*/*/*.cpp
				${CMAKE_SOURCE_DIR}/pch.cpp
)

add_library(OrigoEngine SHARED ${ENGINE_SOURCES})

set_source_files_properties(
				${CMAKE_CURRENT_SOURCE_DIR}/physics/PhysicsWorld.cpp
				PROPERTIES
								SKIP_LINTING ON
)

set_source_files_properties(
				${CMAKE_CURRENT_SOURCE_DIR}/components/systems/RigidbodyComponentPhysicsSystem.cpp
				PROPERTIES
								SKIP_LINTING ON
)

target_include_directories(OrigoEngine PUBLIC
				${CMAKE_SOURCE_DIR}
				${CMAKE_SOURCE_DIR}/vendor/glfw/include
				${CMAKE_SOURCE_DIR}/vendor/glad/include
				${CMAKE_SOURCE_DIR}/vendor/glm
				${CMAKE_SOURCE_DIR}/vendor/assimp/include
				${CMAKE_SOURCE_DIR}/vendor/nanosvg/src
				${CMAKE_SOURCE_DIR}/vendor/stb
				${CMAKE_SOURCE_DIR}/vendor/fastnoise
				${CMAKE_SOURCE_DIR}/vendor/imgui
				${CMAKE_SOURCE_DIR}/vendor/imgui/backends
				${CMAKE_SOURCE_DIR}/vendor/miniaudio
)

target_link_libraries(OrigoEngine PUBLIC
				glfw
				glad
				glm
				assimp
				imgui
				sol2
				jolt

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

target_compile_definitions(OrigoEngine PRIVATE ORIGO_BUILD_DLL)
target_compile_definitions(OrigoEngine PRIVATE ORIGO_SOURCE_ROOT="${CMAKE_SOURCE_DIR}")
target_precompile_headers(OrigoEngine PRIVATE ${CMAKE_SOURCE_DIR}/pch.h)
