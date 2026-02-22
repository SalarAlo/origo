#pragma once

#include "AssetManager.h"

#include "origo/utils/Singleton.h"

namespace Origo {

class DefaultAssetCache : public Singleton<DefaultAssetCache> {
public:
	void CreateAllDefaults();

	AssetHandle GetShader();
	AssetHandle GetTexture();

	AssetHandle GetMaterial();
	AssetHandle GetParticleEmissionDebugMaterial();
	AssetHandle GetParticleMaterial();

private:
	std::optional<AssetHandle> m_Shader;
	std::optional<AssetHandle> m_Texture;
	std::optional<AssetHandle> m_Material;
	std::optional<AssetHandle> m_ParticleEmissionDebugMaterial;
	std::optional<AssetHandle> m_ParticleMaterial;
};

}
