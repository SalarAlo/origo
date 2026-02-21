#include "DefaultAssetCache.h"

#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Material2D.h"
#include "origo/assets/Shader.h"
#include "origo/assets/Texture2D.h"

#include "origo/core/Typedefs.h"

namespace Origo {

DefaultAssetCache& DefaultAssetCache::GetInstance() {
	static DefaultAssetCache instance;
	return instance;
}

void DefaultAssetCache::CreateAllDefaults() {
	GetShader();
	GetTexture();
	GetMaterial();
	GetParticleMaterial();
	GetParticleEmissionDebugMaterial();
}

AssetHandle DefaultAssetCache::GetShader() {
	if (m_Shader.has_value())
		return *m_Shader;

	m_Shader = AssetFactory::CreateSyntheticAsset<Shader>(
	    "Default Shader",
	    UUID::FromHash("ENGINE_DEFAULT_SHADER"));

	auto shader = AssetManager::GetInstance().Get<Shader>(*m_Shader);
	shader->SetSource(MakeScope<ShaderSourceFile>(
	    "./default_resources/default_shader.glsl"));
	shader->Resolve();

	return *m_Shader;
}

AssetHandle DefaultAssetCache::GetTexture() {
	if (m_Texture.has_value())
		return *m_Texture;

	std::vector<unsigned char> whitePixel = { 255, 255, 255, 255 };

	m_Texture = AssetFactory::CreateSyntheticAsset<Texture2D>(
	    "Default White Texture",
	    UUID::FromArbitraryString("DEFAULT_TEXTURE_2D"));

	auto tex = AssetManager::GetInstance().Get<Texture2D>(*m_Texture);
	tex->SetSource(MakeScope<TextureSourceRaw>(
	    1, 1, 4, std::move(whitePixel), false));
	tex->Load();

	return *m_Texture;
}

AssetHandle DefaultAssetCache::GetMaterial() {
	if (m_Material.has_value())
		return *m_Material;

	m_Material = AssetFactory::CreateSyntheticAsset<Material2D>(
	    "Default Material",
	    UUID::FromArbitraryString("DEFAULT_MATERIAL_2D"));

	auto material = AssetManager::GetInstance().Get<Material2D>(*m_Material);
	material->SetShader(GetShader());
	material->SetAlbedo(GetTexture());

	material->SetUniform("u_UseTexture", true);
	material->SetUniform("u_UseLight", true);

	return *m_Material;
}

AssetHandle DefaultAssetCache::GetParticleEmissionDebugMaterial() {
	constexpr Vec3 DEBUG_EMISSION_PARTICLE_COLOR = Vec3 { .1f, .9f, .05f };

	if (m_ParticleEmissionDebugMaterial.has_value())
		return *m_ParticleEmissionDebugMaterial;

	m_ParticleEmissionDebugMaterial = AssetFactory::CreateSyntheticAsset<Material2D>(
	    "Default Particle Material",
	    UUID::FromArbitraryString("DEFAULT_DEBUG_PARTICLE_MATERIAL_2D"));

	auto material = AssetManager::GetInstance().Get<Material2D>(*m_ParticleEmissionDebugMaterial);
	material->SetShader(GetShader());
	material->SetColor(DEBUG_EMISSION_PARTICLE_COLOR);
	material->SetUniform("u_UseTexture", false);
	material->SetUniform("u_UseLight", false);

	return *m_Material;
}

AssetHandle DefaultAssetCache::GetParticleMaterial() {
	constexpr Vec3 PARTICLE_COLOR = Vec3 { 1.0f, 0.0f, 0.0f };

	if (m_ParticleMaterial.has_value())
		return *m_ParticleMaterial;

	m_ParticleMaterial = AssetFactory::CreateSyntheticAsset<Material2D>(
	    "Default Particle Material",
	    UUID::FromArbitraryString("DEFAULT_PARTICLE_MATERIAL_2D"));

	auto material = AssetManager::GetInstance().Get<Material2D>(*m_ParticleMaterial);
	material->SetShader(GetShader());
	material->SetColor(PARTICLE_COLOR);
	material->SetUniform("u_UseTexture", true);
	material->SetUniform("u_UseLight", true);

	return *m_Material;
}

}
