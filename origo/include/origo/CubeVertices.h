#pragma once
// clang-format off
namespace Origo {

static const std::vector<float> CUBE_VERTICES {
    // Front face (0, 0, 1)
    -0.5f, -0.5f,  0.5f,  0, 0, 1,  0.0f, 0.0f,  // bottom-left
     0.5f, -0.5f,  0.5f,  0, 0, 1,  1.0f, 0.0f,  // bottom-right
     0.5f,  0.5f,  0.5f,  0, 0, 1,  1.0f, 1.0f,  // top-right
    -0.5f,  0.5f,  0.5f,  0, 0, 1,  0.0f, 1.0f,  // top-left

    // Back face (0, 0, -1)
    -0.5f, -0.5f, -0.5f,  0, 0,-1,  1.0f, 0.0f,  // bottom-right
     0.5f, -0.5f, -0.5f,  0, 0,-1,  0.0f, 0.0f,  // bottom-left
     0.5f,  0.5f, -0.5f,  0, 0,-1,  0.0f, 1.0f,  // top-left
    -0.5f,  0.5f, -0.5f,  0, 0,-1,  1.0f, 1.0f,  // top-right

    // Right face (1, 0, 0)
     0.5f, -0.5f,  0.5f,  1, 0, 0,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1, 0, 0,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1, 0, 0,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1, 0, 0,  0.0f, 1.0f,

    // Left face (-1, 0, 0)
    -0.5f, -0.5f,  0.5f, -1, 0, 0,  1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, -1, 0, 0,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1, 0, 0,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f, -1, 0, 0,  1.0f, 1.0f,

    // Top face (0, 1, 0)
    -0.5f,  0.5f,  0.5f,  0, 1, 0,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0, 1, 0,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0, 1, 0,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0, 1, 0,  0.0f, 1.0f,

    // Bottom face (0, -1, 0)
    -0.5f, -0.5f,  0.5f,  0,-1, 0,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0,-1, 0,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0,-1, 0,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0,-1, 0,  1.0f, 1.0f,
};

static const std::vector<unsigned int> CUBE_INDICES = {
	0, 1, 2, 2, 3, 0, // Front
	4, 5, 6, 6, 7, 4, // Back
	8, 9, 10, 10, 11, 8, // Right
	12, 13, 14, 14, 15, 12, // Left
	16, 17, 18, 18, 19, 16, // Top
        20, 21, 22, 22, 23, 20 // Bottom
};

// clang-format on
}
