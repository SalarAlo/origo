file(GLOB IMGUI_SRC
    *.cpp
    misc/cpp/imgui_stdlib.cpp
    backends/imgui_impl_glfw.cpp
    backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SRC})

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/cpp
)

target_link_libraries(imgui PUBLIC glfw glad)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
