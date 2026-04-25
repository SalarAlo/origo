#include "origo/assets/MeshSource.h"

#include "magic_enum/magic_enum.hpp"

#include "origo/serialization/ISerializer.h"

namespace Origo {

void MeshSourceRaw::serialize(ISerializer& backend) const {
	backend.begin_object("source");

	backend.begin_array("vertices");
	for (const auto& vertex : m_vertices)
		backend.write(vertex);
	backend.end_array();
	backend.begin_array("indices");
	for (const auto& i : m_indices)
		backend.write(static_cast<int>(i));
	backend.end_array();
	backend.write("type", magic_enum::enum_name(get_type()));

	backend.end_object();
}

MeshSource* MeshSourceRaw::deserialize(ISerializer& backend) const {
	// Left empty on purpose
	return nullptr;
}

void MeshSourcePrimitiveShape::serialize(ISerializer& backend) const {
	backend.begin_object("source");

	backend.write("shape_type", magic_enum::enum_name(m_shape));
	backend.write("type", magic_enum::enum_name(get_type()));

	backend.end_object();
}

MeshSource* MeshSourcePrimitiveShape::deserialize(ISerializer& backend) const {
	// Left empty on purpose (rebuilt everytime anyways (see PrimitiveShapeCache.h))
	return nullptr;
}

void MeshSourceExternal::serialize(ISerializer& backend) const {
	backend.begin_object("source");

	backend.write("model_path", m_model_path);
	backend.write("mesh_index", static_cast<int>(m_mesh_index));
	backend.write("type", magic_enum::enum_name(get_type()));

	backend.end_object();
}

MeshSource* MeshSourceExternal::deserialize(ISerializer& backend) const {
	// Left empty on purpose (should not serialize)
	return nullptr;
}

}
