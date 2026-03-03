cmake_minimum_required(VERSION 3.16)

# Optional, aber sauber: eigenes Projekt für Tests
project(OrigoTests LANGUAGES CXX)

add_executable(OrigoTests
	src/Main.cpp
	src/RIDTest.cpp
	src/UUIDTest.cpp
)

target_link_libraries(OrigoTests
    PRIVATE
        OrigoEngine
        doctest::doctest
)

target_include_directories(OrigoTests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/origo/src
)

target_compile_features(OrigoTests PRIVATE cxx_std_23)

# Optional: gleiche PCH wie Engine (nur wenn du willst)
target_precompile_headers(OrigoTests PRIVATE
    ${CMAKE_SOURCE_DIR}/pch.h
)

set_target_properties(OrigoTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${output_dir}
)

# Optional, aber professionell
enable_testing()
add_test(NAME OrigoTests COMMAND OrigoTests)
