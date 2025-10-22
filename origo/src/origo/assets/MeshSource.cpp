#include "magic_enum/magic_enum.hpp"
#include "nlohmann/json.hpp"
#include "origo/assets/MeshSource.h"
#include "nlohmann/json_fwd.hpp"
#include "origo/assets/PrimitiveShape.h"

namespace Origo {

nlohmann::json MeshSourceRaw::Serialize() const {
	nlohmann::json j;

	j["vertices"] = m_Vertices;
	j["indices"] = m_Indices;
	j["src"] = magic_enum::enum_name(GetType());

	return j;
}

Ref<MeshSource> MeshSourceRaw::Deserialize(const nlohmann::json& j) const {
	std::vector<float> vertices { j["vertices"].get<std::vector<float>>() };
	std::vector<unsigned int> indices { j["indices"].get<std::vector<unsigned int>>() };

	return MakeRef<MeshSourceRaw>(vertices, indices);
}

nlohmann::json MeshSourcePrimitiveShape::Serialize() const {
	nlohmann::json j;

	j["shape_type"] = magic_enum::enum_name(m_Shape);
	j["src"] = magic_enum::enum_name(GetType());

	return j;
}

Ref<MeshSource> MeshSourcePrimitiveShape::Deserialize(const nlohmann::json& j) const {
	std::string typeName { j["shape_type"] };
	std::optional<PrimitiveShape> shape { magic_enum::enum_cast<PrimitiveShape>(typeName) };
	if (shape.has_value())
		return MakeRef<MeshSourcePrimitiveShape>(shape.value());
	else
		return nullptr;
}

nlohmann::json MeshSourceExternal::Serialize() const {
	nlohmann::json j;

	j["model_path"] = m_ModelPath;
	j["mesh_index"] = m_MeshIndex;
	j["src"] = magic_enum::enum_name(GetType());

	return j;
}

Ref<MeshSource> MeshSourceExternal::Deserialize(const nlohmann::json& j) const {
	return nullptr;
}

}
