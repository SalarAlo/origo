#pragma once

#include "origo/assets/PrimitiveShape.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {

enum class MeshSourceType {
	Raw,
	Primitive,
	External
};

class MeshSource {
public:
	virtual ~MeshSource() = default;
	virtual MeshData get_data() const = 0;

	virtual void serialize(ISerializer& backend) const = 0;
	virtual MeshSource* deserialize(ISerializer& backend) const = 0;

	virtual MeshSourceType get_type() const = 0;

private:
};

class MeshSourceRaw : public MeshSource {
public:
	MeshSourceRaw(const std::vector<float>& Vertices, const std::vector<unsigned int>& Indices)
	    : m_vertices(Vertices)
	    , m_indices(Indices) {
	}

	MeshData get_data() const override {
		static MeshData data { m_vertices, m_indices };
		return data;
	}

	MeshSourceType get_type() const override {
		return MeshSourceType::Raw;
	}

	void serialize(ISerializer& backend) const override;
	MeshSource* deserialize(ISerializer& backend) const override;

private:
	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indices;
};

class MeshSourcePrimitiveShape : public MeshSource {
public:
	MeshSourcePrimitiveShape(PrimitiveShape shape)
	    : m_shape(shape) {
	}

	MeshData get_data() const override {
		static auto data { get_data_from_shape(m_shape) };
		return data;
	}

	MeshSourceType get_type() const override {
		return MeshSourceType::Primitive;
	}

	void serialize(ISerializer& backend) const override;
	MeshSource* deserialize(ISerializer& backend) const override;

private:
	PrimitiveShape m_shape {};
};

class MeshSourceExternal : public MeshSource {
public:
	MeshSourceExternal(std::string_view modelPath, size_t meshIndex, const MeshData& data)
	    : m_model_path(modelPath)
	    , m_mesh_index(meshIndex)
	    , m_data(data) { }

	std::string get_path() const { return m_model_path; }
	size_t get_mesh_index() const { return m_mesh_index; }
	MeshData get_data() const override { return m_data; }

	MeshSourceType get_type() const override {
		return MeshSourceType::External;
	}

	void serialize(ISerializer& backend) const override;
	MeshSource* deserialize(ISerializer& backend) const override;

private:
	std::string m_model_path {};
	MeshData m_data {};
	size_t m_mesh_index {};
};

}
