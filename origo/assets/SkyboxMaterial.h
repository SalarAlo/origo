#pragma once

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"
#include <glm/glm.hpp>

namespace Origo {

class SkyboxMaterial final : public Asset {
public:
	SkyboxMaterial(AssetHandle shader, AssetHandle cubemap);

	static AssetType get_class_asset_type() { return AssetType::SkyboxMaterial; }
	AssetType get_asset_type() const override { return get_class_asset_type(); }

	void bind(const glm::mat4& projection, const glm::mat4& view);

private:
	AssetHandle m_shader;
	AssetHandle m_cubemap;
};

}
