#pragma once

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManagerFast.h"
#include <glm/glm.hpp>

namespace Origo {

class SkyboxMaterial final : public Asset {
public:
	SkyboxMaterial(AssetHandle shader, AssetHandle cubemap);

	static AssetType GetClassAssetType() { return AssetType::SkyboxMaterial; }
	AssetType GetAssetType() const override { return GetClassAssetType(); }

	void Bind(const glm::mat4& projection, const glm::mat4& view);

private:
	AssetHandle m_Shader;
	AssetHandle m_Cubemap;
};

}
