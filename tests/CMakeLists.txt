cmake_minimum_required(VERSION 3.16)

project(OrigoTests LANGUAGES CXX)

add_executable(OrigoTests
	src/CameraComponentSerializationTest.cpp
	src/Main.cpp
	src/JsonSerializerTest.cpp
	src/PrefabSerializationTest.cpp
	src/RIDTest.cpp
	src/SceneAssetSerializationTest.cpp
	src/SceneSerializationTest.cpp
	src/ScriptAssetSerializationTest.cpp
	src/UUIDTest.cpp
)

target_link_libraries(OrigoTests
				PRIVATE
								OrigoEngine
								doctest::doctest
)

target_include_directories(OrigoTests
				PRIVATE
								${CMAKE_SOURCE_DIR}
)

target_compile_features(OrigoTests PRIVATE cxx_std_23)

target_precompile_headers(OrigoTests PRIVATE
				${CMAKE_SOURCE_DIR}/pch.h
)

set_target_properties(OrigoTests PROPERTIES
				RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${output_dir}
)

enable_testing()
add_test(NAME OrigoTests COMMAND OrigoTests)
