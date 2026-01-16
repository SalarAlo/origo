#pragma once

#include "AssetManager.h"

namespace Origo {

class DefaultAssetCache {
public:
	static DefaultAssetCache& GetInstance();

	void CreateAllDefaults();

	AssetHandle GetShader();
	AssetHandle GetTexture();
	AssetHandle GetMaterial();

private:
	DefaultAssetCache() = default;

	std::optional<AssetHandle> m_Shader;
	std::optional<AssetHandle> m_Texture;
	std::optional<AssetHandle> m_Material;
};

}
