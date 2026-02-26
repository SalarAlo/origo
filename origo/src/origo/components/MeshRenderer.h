#pragma once

#include "Component.h"

#include "origo/assets/AssetManager.h"

namespace Origo {

class MeshRendererComponent : public Component {
public:
	MeshRendererComponent() = default;
	MeshRendererComponent(OptionalAssetHandle material, OptionalAssetHandle mesh);

	std::string get_component_name() const override { return "MeshRenderer"; }

	OptionalAssetHandle MaterialHandle { std::nullopt };
	OptionalAssetHandle MeshHandle { std::nullopt };
};

}
