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

            v.push_back( x);
	    v.push_back( y);
	    v.push_back( z);

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
            // CCW from outside
            if (i != 0) {
                idx.push_back(k1);
                idx.push_back(k1 + 1);
                idx.push_back(k2);
            }

            if (i != (stacks - 1)) {
                idx.push_back(k1 + 1);
                idx.push_back(k2 + 1);
                idx.push_back(k2);
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

static const std::vector<float> CONE_VERTICES = [] {
    const int sectors = 32;
    const float PI = 3.14159265359f;

    const float radius     = 0.5f;
    const float halfHeight = 0.5f;
    const float height     = 2.0f * halfHeight;

    auto normalize3 = [](float x, float y, float z) {
        float len = sqrtf(x*x + y*y + z*z);
        if (len <= 0.000001f) return std::array<float,3>{0.0f, 1.0f, 0.0f};
        float inv = 1.0f / len;
        return std::array<float,3>{x*inv, y*inv, z*inv};
    };

    std::vector<float> v;
    // layout:
    // tip (1) + baseCenter (1) + sideRing (sectors+1) + baseRing (sectors+1)
    v.reserve((2 + (sectors + 1) * 2) * 8);

    // --- Tip vertex (index 0) ---
    v.insert(v.end(), {
        0.0f,  halfHeight, 0.0f,
        0.0f,  1.0f,       0.0f,
        0.5f,  1.0f
    });

    // --- Base center (index 1) ---
    v.insert(v.end(), {
        0.0f, -halfHeight, 0.0f,
        0.0f, -1.0f,       0.0f,
        0.5f,  0.5f
    });

    // --- Side ring vertices (indices 2 .. 2+sectors) ---
    // smooth-ish cone normals: normalize( x, radius/height, z )
    const float ny = radius / height;

    for (int i = 0; i <= sectors; i++) {
        float t = (float)i / (float)sectors;
        float a = t * 2.0f * PI;

        float x = radius * cosf(a);
        float z = radius * sinf(a);

        auto n = normalize3(x, ny, z);

        v.push_back(x);
        v.push_back(-halfHeight);
        v.push_back(z);

        v.push_back(n[0]);
        v.push_back(n[1]);
        v.push_back(n[2]);

        v.push_back(t);      // U wraps around
        v.push_back(0.0f);   // V at base of side
    }

    // --- Base ring vertices (separate duplicates; flat normals) ---
    // indices: baseRingStart .. baseRingStart+sectors
    for (int i = 0; i <= sectors; i++) {
        float t = (float)i / (float)sectors;
        float a = t * 2.0f * PI;

        float x = radius * cosf(a);
        float z = radius * sinf(a);

        // map circle into [0..1] UV space
        float u = (x / radius) * 0.5f + 0.5f;
        float w = (z / radius) * 0.5f + 0.5f;

        v.push_back(x);
        v.push_back(-halfHeight);
        v.push_back(z);

        v.push_back(0.0f);
        v.push_back(-1.0f);
        v.push_back(0.0f);

        v.push_back(u);
        v.push_back(w);
    }

    return v;
}();

static const std::vector<unsigned int> CONE_INDICES = [] {
    const int sectors = 32;

    std::vector<unsigned int> idx;
    idx.reserve(sectors * 6);

    const unsigned int tipIndex        = 0;
    const unsigned int baseCenterIndex = 1;
    const unsigned int sideRingStart   = 2;
    const unsigned int baseRingStart   = sideRingStart + (sectors + 1);

    // --- Side triangles (CCW from outside) ---
    for (int i = 0; i < sectors; i++) {
        idx.push_back(tipIndex);
        idx.push_back(sideRingStart + i + 1);
        idx.push_back(sideRingStart + i);
    }

    // --- Base triangles (CCW when viewed from outside i.e. from -Y) ---
    for (int i = 0; i < sectors; i++) {
        idx.push_back(baseCenterIndex);
        idx.push_back(baseRingStart + i);
        idx.push_back(baseRingStart + i + 1);
    }

    return idx;
}();

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
	case PrimitiveShape::Cone:
		return { CONE_VERTICES, CONE_INDICES };
	}

	return { {}, {} };
}

}
