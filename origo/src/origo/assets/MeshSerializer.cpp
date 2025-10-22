#include "origo/assets/MeshSerializer.h"
#include "nlohmann/json.hpp"
#include "origo/assets/Mesh.h"
#include <memory>

namespace Origo {
nlohmann::json MeshSerializer::Serialize(const Ref<Asset>& asset) const {
	const auto& mesh { std::dynamic_pointer_cast<Mesh>(asset) };
	nlohmann::json j;
	j["src"] = mesh->GetSource()->Serialize();
	return j;
}

Ref<Asset> MeshSerializer::Deserialize(const nlohmann::json& j) const {
	return nullptr;
}
}
