#pragma once

#include "origo/assets/Texture2D.h"
#include "origo/assets/material/Material.h"

namespace Origo {

class TerrainMaterial : public Material {
public:
	enum class TextureSlot : int {
		GroundAlbedo = 0,
		GroundNormal = 1,
		GroundPacked = 2,
		RockAlbedo = 3,
		RockNormal = 4,
		RockPacked = 5,
		SandAlbedo = 6,
		SandNormal = 7,
		SandPacked = 8,
		SnowAlbedo = 9,
		SnowNormal = 10,
		SnowPacked = 11,
		MicroNormal = 12
	};

	TerrainMaterial();

	void resolve() override;
	void bind() override;

	AssetType get_asset_type() const override { return AssetType::TerrainMaterial; }
	static AssetType get_class_asset_type() { return AssetType::TerrainMaterial; }

	TerrainMaterial& set_shader(const OptionalAssetHandle& shader) {
		Material::set_shader(shader);
		return *this;
	}

	TerrainMaterial& set_ground_albedo(const OptionalAssetHandle& handle);
	TerrainMaterial& set_ground_normal(const OptionalAssetHandle& handle);
	TerrainMaterial& set_ground_packed(const OptionalAssetHandle& handle);
	TerrainMaterial& set_rock_albedo(const OptionalAssetHandle& handle);
	TerrainMaterial& set_rock_normal(const OptionalAssetHandle& handle);
	TerrainMaterial& set_rock_packed(const OptionalAssetHandle& handle);
	TerrainMaterial& set_sand_albedo(const OptionalAssetHandle& handle);
	TerrainMaterial& set_sand_normal(const OptionalAssetHandle& handle);
	TerrainMaterial& set_sand_packed(const OptionalAssetHandle& handle);
	TerrainMaterial& set_snow_albedo(const OptionalAssetHandle& handle);
	TerrainMaterial& set_snow_normal(const OptionalAssetHandle& handle);
	TerrainMaterial& set_snow_packed(const OptionalAssetHandle& handle);
	TerrainMaterial& set_micro_normal(const OptionalAssetHandle& handle);

private:
	void resolve_texture(Texture2D*& destination, const OptionalAssetHandle& handle);
	void bind_texture(Texture2D* texture, TextureSlot slot, const char* uniform_name);

private:
	OptionalAssetHandle m_ground_albedo_handle {};
	OptionalAssetHandle m_ground_normal_handle {};
	OptionalAssetHandle m_ground_packed_handle {};
	OptionalAssetHandle m_rock_albedo_handle {};
	OptionalAssetHandle m_rock_normal_handle {};
	OptionalAssetHandle m_rock_packed_handle {};
	OptionalAssetHandle m_sand_albedo_handle {};
	OptionalAssetHandle m_sand_normal_handle {};
	OptionalAssetHandle m_sand_packed_handle {};
	OptionalAssetHandle m_snow_albedo_handle {};
	OptionalAssetHandle m_snow_normal_handle {};
	OptionalAssetHandle m_snow_packed_handle {};
	OptionalAssetHandle m_micro_normal_handle {};

	Texture2D* m_ground_albedo {};
	Texture2D* m_ground_normal {};
	Texture2D* m_ground_packed {};
	Texture2D* m_rock_albedo {};
	Texture2D* m_rock_normal {};
	Texture2D* m_rock_packed {};
	Texture2D* m_sand_albedo {};
	Texture2D* m_sand_normal {};
	Texture2D* m_sand_packed {};
	Texture2D* m_snow_albedo {};
	Texture2D* m_snow_normal {};
	Texture2D* m_snow_packed {};
	Texture2D* m_micro_normal {};
};

}
