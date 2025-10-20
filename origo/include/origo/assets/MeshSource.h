#pragma once

#include "origo/assets/PrimitiveShape.h"
#include "origo/core/Logger.h"

namespace Origo {

class MeshSource {
public:
	virtual MeshData GetData() const = 0;

private:
};

class MeshSourceRaw : public MeshSource {
public:
	MeshSourceRaw(const std::vector<float>& Vertices, const std::vector<unsigned int>& Indices)
	    : m_Vertices(Vertices)
	    , m_Indices(Indices) {
	}

	MeshData GetData() const {
		static MeshData data { m_Vertices, m_Indices };
		return data;
	}

private:
	std::vector<float> m_Vertices;
	std::vector<unsigned int> m_Indices;
};

class MeshSourcePrimitiveShape : public MeshSource {
public:
	MeshSourcePrimitiveShape(PrimitiveShape shape)
	    : m_Shape(shape) {
	}

	MeshData GetData() const {
		static auto data { GetDataFromShape(m_Shape) };
		ORG_INFO("The cube has {} amount of vertices", data.Vertices.size());
		return data;
	}

private:
	PrimitiveShape m_Shape;
};

}
