project(EditorApp LANGUAGES C CXX)

file(GLOB_RECURSE EDITOR_SOURCES src/*.cpp)

add_executable(Editor ${EDITOR_SOURCES})

target_link_libraries(Editor PRIVATE OrigoEngine)

target_include_directories(Editor PRIVATE ${CMAKE_SOURCE_DIR}/origo/include)
