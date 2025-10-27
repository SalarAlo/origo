#include "magic_enum/magic_enum.hpp"
#include "origo/assets/MeshSource.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

void MeshSourceRaw::Serialize(ISerializer& backend) const {
	backend.BeginObject("source");

	backend.BeginArray("vertices");
	for (const auto& vertex : m_Vertices)
		backend.Write(vertex);
	backend.EndArray();
	backend.BeginArray("indices");
	for (const auto& i : m_Indices)
		backend.Write(static_cast<int>(i));
	backend.EndArray();
	backend.Write("type", magic_enum::enum_name(GetType()));

	backend.EndObject();
}

Ref<MeshSource> MeshSourceRaw::Deserialize(ISerializer& backend) const {
	// TODO:
	return nullptr;
}

void MeshSourcePrimitiveShape::Serialize(ISerializer& backend) const {
	backend.BeginObject("source");

	backend.Write("shape_type", magic_enum::enum_name(m_Shape));
	backend.Write("type", magic_enum::enum_name(GetType()));

	backend.EndObject();
}

Ref<MeshSource> MeshSourcePrimitiveShape::Deserialize(ISerializer& backend) const {
	// TODO:
	return nullptr;
}

void MeshSourceExternal::Serialize(ISerializer& backend) const {
	backend.BeginObject("source");

	backend.Write("model_path", m_ModelPath);
	backend.Write("mesh_index", static_cast<int>(m_MeshIndex));
	backend.Write("type", magic_enum::enum_name(GetType()));

	backend.EndObject();
}

Ref<MeshSource> MeshSourceExternal::Deserialize(ISerializer& backend) const {
	// TODO:
	return nullptr;
}

}
