#include "DefaultAssetCache.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/Material2D.h"
#include "origo/assets/Shader.h"
#include "origo/assets/Texture2D.h"

namespace Origo {

DefaultAssetCache& DefaultAssetCache::GetInstance() {
	static DefaultAssetCache instance;
	return instance;
}

void DefaultAssetCache::CreateAllDefaults() {
	GetShader();
	GetTexture();
	GetMaterial();
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

	return *m_Material;
}

}
