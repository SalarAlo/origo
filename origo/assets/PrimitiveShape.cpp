#include "origo/assets/PrimitiveShape.h"

#pragma region SHAPE_DEFS
// clang-format off

static const std::vector<float> cube_vertices {
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


static const std::vector<unsigned int> cube_indices = {
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

static const std::vector<float> wire_cube_vertices {
    // X,     Y,     Z,      NX, NY, NZ,   U,    V
    -0.5f, -0.5f, -0.5f,    0,  0, -1,    0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,    0,  0, -1,    1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,    0,  0, -1,    1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,    0,  0, -1,    0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,    0,  0,  1,    0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,    0,  0,  1,    1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,    0,  0,  1,    1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,    0,  0,  1,    0.0f, 1.0f,
};

static const std::vector<unsigned int> wire_cube_indices {
    // back face
    0, 1,
    1, 2,
    2, 3,
    3, 0,
    // front face
    4, 5,
    5, 6,
    6, 7,
    7, 4,
    // connecting edges
    0, 4,
    1, 5,
    2, 6,
    3, 7,
};

static const std::vector<float> wire_cone_vertices = [] {
    const int sectors = 32;
    const float pi = 3.14159265359f;
    const float radius = 0.5f;
    const float half_height = 0.5f;

    std::vector<float> v;
    v.reserve((sectors + 2) * 8);

    // tip
    v.insert(v.end(), {
        0.0f,  half_height, 0.0f,
        0.0f,  1.0f,        0.0f,
        0.5f,  1.0f
    });

    // base ring
    for (int i = 0; i <= sectors; i++) {
        const float t = static_cast<float>(i) / static_cast<float>(sectors);
        const float a = t * 2.0f * pi;
        const float x = radius * cosf(a);
        const float z = radius * sinf(a);

        v.push_back(x);
        v.push_back(-half_height);
        v.push_back(z);

        v.push_back(0.0f);
        v.push_back(-1.0f);
        v.push_back(0.0f);

        v.push_back(t);
        v.push_back(0.0f);
    }

    return v;
}();

static const std::vector<unsigned int> wire_cone_indices = [] {
    const int sectors = 32;
    const unsigned int tip_index = 0;
    const unsigned int base_ring_start = 1;
    const int side_segments = 8;
    const int sector_step = sectors / side_segments;

    std::vector<unsigned int> idx;
    idx.reserve((sectors * 2 + side_segments * 2) * 2);

    for (int i = 0; i < sectors; i++) {
        idx.push_back(base_ring_start + i);
        idx.push_back(base_ring_start + i + 1);
    }

    for (int i = 0; i < sectors; i += sector_step) {
        idx.push_back(tip_index);
        idx.push_back(base_ring_start + i);
    }

    return idx;
}();

static const std::vector<float> triangle_vertices {
    0.0f,  0.5f, 0.0f,   0, 0, 1,  0.5f, 1.0f,
   -0.5f, -0.5f, 0.0f,   0, 0, 1,  0.0f, 0.0f,
    0.5f, -0.5f, 0.0f,   0, 0, 1,  1.0f, 0.0f
};

static const std::vector<unsigned int> triangle_indices {
    0, 1, 2
};

static const std::vector<float> sphere_vertices = [] {
    const int stacks  = 32;
    const int sectors = 32;

    const float pi = 3.14159265359f;

    std::vector<float> v;
    v.reserve(stacks * sectors * 8);

    for (int i = 0; i <= stacks; i++) {
        float stack_angle = pi/2 - (float)i / stacks * pi;   
        float xy = cosf(stack_angle);
        float y  = sinf(stack_angle);

        for (int j = 0; j <= sectors; j++) {
            float sector_angle = (float)j / sectors * 2 * pi; 

            float x = xy * cosf(sector_angle);
            float z = xy * sinf(sector_angle);

            float u = (float)j / sectors;
            float v_coord = (float)i / stacks;

            v.push_back( x * 0.5f);
	    v.push_back( y * 0.5f);
	    v.push_back( z * 0.5f);

            v.push_back(x);
            v.push_back(y);
            v.push_back(z);

            v.push_back(u);
            v.push_back(v_coord);
        }
    }

    return v;
}();

static const std::vector<unsigned int> sphere_indices = [] {
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

static const std::vector<float> quad_vertices {
    // X,     Y,    Z,     NX, NY, NZ,   U,    V
    -0.5f,  0.0f, -0.5f,  0,  1,  0,    0.0f, 0.0f, // bottom-left
     0.5f,  0.0f, -0.5f,  0,  1,  0,    1.0f, 0.0f, // bottom-right
     0.5f,  0.0f,  0.5f,  0,  1,  0,    1.0f, 1.0f, // top-right
    -0.5f,  0.0f,  0.5f,  0,  1,  0,    0.0f, 1.0f  // top-left
};

static const std::vector<unsigned int> quad_indices {
    0, 1, 2,
    2, 3, 0
};

static const std::vector<float> cone_vertices = [] {
    const int sectors = 32;
    const float pi = 3.14159265359f;

    const float radius     = 0.5f;
    const float half_height = 0.5f;
    const float height     = 2.0f * half_height;

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
        0.0f,  half_height, 0.0f,
        0.0f,  1.0f,       0.0f,
        0.5f,  1.0f
    });

    // --- Base center (index 1) ---
    v.insert(v.end(), {
        0.0f, -half_height, 0.0f,
        0.0f, -1.0f,       0.0f,
        0.5f,  0.5f
    });

    // --- Side ring vertices (indices 2 .. 2+sectors) ---
    // smooth-ish cone normals: normalize( x, radius/height, z )
    const float ny = radius / height;

    for (int i = 0; i <= sectors; i++) {
        float t = (float)i / (float)sectors;
        float a = t * 2.0f * pi;

        float x = radius * cosf(a);
        float z = radius * sinf(a);

        auto n = normalize3(x, ny, z);

        v.push_back(x);
        v.push_back(-half_height);
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
        float a = t * 2.0f * pi;

        float x = radius * cosf(a);
        float z = radius * sinf(a);

        // map circle into [0..1] UV space
        float u = (x / radius) * 0.5f + 0.5f;
        float w = (z / radius) * 0.5f + 0.5f;

        v.push_back(x);
        v.push_back(-half_height);
        v.push_back(z);

        v.push_back(0.0f);
        v.push_back(-1.0f);
        v.push_back(0.0f);

        v.push_back(u);
        v.push_back(w);
    }

    return v;
}();

static const std::vector<unsigned int> cone_indices = [] {
    const int sectors = 32;

    std::vector<unsigned int> idx;
    idx.reserve(sectors * 6);

    const unsigned int tip_index        = 0;
    const unsigned int base_center_index = 1;
    const unsigned int side_ring_start   = 2;
    const unsigned int base_ring_start   = side_ring_start + (sectors + 1);

    // --- Side triangles (CCW from outside) ---
    for (int i = 0; i < sectors; i++) {
        idx.push_back(tip_index);
        idx.push_back(side_ring_start + i + 1);
        idx.push_back(side_ring_start + i);
    }

    // --- Base triangles (CCW when viewed from outside i.e. from -Y) ---
    for (int i = 0; i < sectors; i++) {
        idx.push_back(base_center_index);
        idx.push_back(base_ring_start + i);
        idx.push_back(base_ring_start + i + 1);
    }

    return idx;
}();

// clang-format on

#pragma endregion

namespace Origo {
MeshData get_data_from_shape(PrimitiveShape shape) {
	switch (shape) {
	case PrimitiveShape::Cube:
		return { cube_vertices, cube_indices };
	case PrimitiveShape::WireCube:
		return { wire_cube_vertices, wire_cube_indices };
	case PrimitiveShape::WireCone:
		return { wire_cone_vertices, wire_cone_indices };
	case PrimitiveShape::Triangle:
		return { triangle_vertices, triangle_indices };
	case PrimitiveShape::Sphere:
		return { sphere_vertices, sphere_indices };
	case PrimitiveShape::Quad:
		return { quad_vertices, quad_indices };
	case PrimitiveShape::Cone:
		return { cone_vertices, cone_indices };
	}

	return { {}, {} };
}

}
