#include "origo/assets/PrimitiveShape.h"

#pragma region SHAPE_DEFS
// clang-format off

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
    // Front face (CCW from outside)
    0, 1, 2, 
    2, 3, 0,
    
    // Back face (CCW from outside)
    6, 5, 4, 
    4, 7, 6,
    
    // Right face (CCW from outside)
    8, 9, 10, 
    10, 11, 8,
    
    // Left face (CCW from outside)
    14, 13, 12, 
    12, 15, 14,
    
    // Top face (CCW from outside)
    16, 17, 18, 
    18, 19, 16,
    
    // Bottom face (CCW from outside)
    22, 21, 20, 
    20, 23, 22
};

static const std::vector<float> TRIANGLE_VERTICES {
    0.0f,  0.5f, 0.0f,   0, 0, 1,  0.5f, 1.0f,
   -0.5f, -0.5f, 0.0f,   0, 0, 1,  0.0f, 0.0f,
    0.5f, -0.5f, 0.0f,   0, 0, 1,  1.0f, 0.0f
};

static const std::vector<unsigned int> TRIANGLE_INDICES {
    0, 1, 2
};

static const std::vector<float> SPHERE_VERTICES = [] {
    const int stacks  = 32;
    const int sectors = 32;

    const float PI = 3.14159265359f;

    std::vector<float> v;
    v.reserve(stacks * sectors * 8);

    for (int i = 0; i <= stacks; i++) {
        float stackAngle = PI/2 - (float)i / stacks * PI;   
        float xy = cosf(stackAngle);
        float y  = sinf(stackAngle);

        for (int j = 0; j <= sectors; j++) {
            float sectorAngle = (float)j / sectors * 2 * PI; 

            float x = xy * cosf(sectorAngle);
            float z = xy * sinf(sectorAngle);

            float u = (float)j / sectors;
            float vCoord = (float)i / stacks;

            v.push_back(x);
            v.push_back(y);
            v.push_back(z);

            v.push_back(x);
            v.push_back(y);
            v.push_back(z);

            v.push_back(u);
            v.push_back(vCoord);
        }
    }

    return v;
}();

static const std::vector<unsigned int> SPHERE_INDICES = [] {
    const int stacks  = 32;
    const int sectors = 32;

    std::vector<unsigned int> idx;
    idx.reserve(stacks * sectors * 6);

    for (int i = 0; i < stacks; i++) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; j++, k1++, k2++) {
            if (i != 0) {
                idx.push_back(k1);
                idx.push_back(k2);
                idx.push_back(k1 + 1);
            }

            if (i != (stacks - 1)) {
                idx.push_back(k1 + 1);
                idx.push_back(k2);
                idx.push_back(k2 + 1);
            }
        }
    }

    return idx;
}();

static const std::vector<float> QUAD_VERTICES {
    // X,     Y,    Z,     NX, NY, NZ,   U,    V
    -0.5f,  0.0f, -0.5f,  0,  1,  0,    0.0f, 0.0f, // bottom-left
     0.5f,  0.0f, -0.5f,  0,  1,  0,    1.0f, 0.0f, // bottom-right
     0.5f,  0.0f,  0.5f,  0,  1,  0,    1.0f, 1.0f, // top-right
    -0.5f,  0.0f,  0.5f,  0,  1,  0,    0.0f, 1.0f  // top-left
};

static const std::vector<unsigned int> QUAD_INDICES {
    0, 1, 2,
    2, 3, 0
};

// clang-format on

#pragma endregion

namespace Origo {
MeshData GetDataFromShape(PrimitiveShape shape) {
	switch (shape) {
	case PrimitiveShape::Cube:
		return { CUBE_VERTICES, CUBE_INDICES };
	case PrimitiveShape::Triangle:
		return { TRIANGLE_VERTICES, TRIANGLE_INDICES };
	case PrimitiveShape::Sphere:
		return { SPHERE_VERTICES, SPHERE_INDICES };
	case PrimitiveShape::Quad:
		return { QUAD_VERTICES, QUAD_INDICES };
	default:
		return { {}, {} };
	}
}

}
