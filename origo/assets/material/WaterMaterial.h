#pragma once

#include "origo/assets/Texture2D.h"
#include "origo/assets/material/Material.h"

namespace Origo {

class WaterMaterial : public Material {
public:
	enum class TextureSlot : int {
		WaveA = 0,
		WaveB = 1
	};

	WaterMaterial();

	void resolve() override;
	void bind() override;

	AssetType get_asset_type() const override { return AssetType::WaterMaterial; }
	static AssetType get_class_asset_type() { return AssetType::WaterMaterial; }

	WaterMaterial& set_shader(const OptionalAssetHandle& shader) {
		Material::set_shader(shader);
		return *this;
	}

	WaterMaterial& set_wave_a(const OptionalAssetHandle& handle);
	WaterMaterial& set_wave_b(const OptionalAssetHandle& handle);

private:
	void resolve_texture(Texture2D*& destination, const OptionalAssetHandle& handle);

private:
	OptionalAssetHandle m_wave_a_handle {};
	OptionalAssetHandle m_wave_b_handle {};
	Texture2D* m_wave_a {};
	Texture2D* m_wave_b {};
};

}
