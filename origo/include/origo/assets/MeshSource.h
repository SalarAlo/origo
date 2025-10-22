#pragma once

#include "origo/assets/PrimitiveShape.h"
#include "nlohmann/json_fwd.hpp"
#include "origo/core/Logger.h"

namespace Origo {

enum class MeshSourceType {
	Raw,
	Primitive,
	External
};

class MeshSource {
public:
	virtual MeshData GetData() const = 0;

	virtual nlohmann::json Serialize() const = 0;
	virtual Ref<MeshSource> Deserialize(const nlohmann::json&) const = 0;

	virtual MeshSourceType GetType() const = 0;

private:
};

class MeshSourceRaw : public MeshSource {
public:
	MeshSourceRaw(const std::vector<float>& Vertices, const std::vector<unsigned int>& Indices)
	    : m_Vertices(Vertices)
	    , m_Indices(Indices) {
	}

	MeshData GetData() const override {
		static MeshData data { m_Vertices, m_Indices };
		return data;
	}

	MeshSourceType GetType() const override {
		return MeshSourceType::Raw;
	}

	nlohmann::json Serialize() const override;
	Ref<MeshSource> Deserialize(const nlohmann::json&) const override;

private:
	std::vector<float> m_Vertices;
	std::vector<unsigned int> m_Indices;
};

class MeshSourcePrimitiveShape : public MeshSource {
public:
	MeshSourcePrimitiveShape(PrimitiveShape shape)
	    : m_Shape(shape) {
	}

	MeshData GetData() const override {
		static auto data { GetDataFromShape(m_Shape) };
		return data;
	}

	MeshSourceType GetType() const override {
		return MeshSourceType::Primitive;
	}

	nlohmann::json Serialize() const override;
	Ref<MeshSource> Deserialize(const nlohmann::json&) const override;

private:
	PrimitiveShape m_Shape {};
};

class MeshSourceExternal : public MeshSource {
public:
	MeshSourceExternal(std::string_view modelPath, size_t meshIndex, const MeshData& data)
	    : m_ModelPath(modelPath)
	    , m_MeshIndex(meshIndex)
	    , m_Data(data) { }

	std::string GetPath() const { return m_ModelPath; }
	size_t GetMeshIndex() const { return m_MeshIndex; }
	MeshData GetData() const override { return m_Data; }

	MeshSourceType GetType() const override {
		return MeshSourceType::External;
	}

	nlohmann::json Serialize() const override;
	Ref<MeshSource> Deserialize(const nlohmann::json&) const override;

private:
	std::string m_ModelPath {};
	MeshData m_Data {};
	size_t m_MeshIndex {};
};

}
