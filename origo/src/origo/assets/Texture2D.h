#pragma once

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/TextureSource.h"

namespace Origo {

enum class TextureType {
	Albedo,
	Normal,
	MetallicRoughness,
	Ao,
	Emissive,
	UI
};

class Texture2D : public Asset {
public:
	Texture2D(TextureType type = TextureType::Albedo);

	void set_source(Scope<TextureSource> src);
	TextureSource* get_source() const { return m_source.get(); }

	void bind(AssetHandle shaderId) const;

	AssetType get_asset_type() const override { return AssetType::Texture2D; }
	static AssetType get_class_asset_type() { return AssetType::Texture2D; }

	TextureType get_texture_type() const { return m_texture_type; }
	void set_texture_type(TextureType type) { m_texture_type = type; }
	GLuint get_renderer_id() { return m_texture_id; }

	void resolve() override;

	void load();

private:
	void init_texture(const TextureInitialisationData& data);

private:
	Scope<TextureSource> m_source;

	GLuint m_texture_id = 0;
	bool m_loaded { false };
	TextureType m_texture_type;
};
}
