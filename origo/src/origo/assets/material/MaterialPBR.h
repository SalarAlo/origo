#pragma once

#include "origo/assets/AssetManager.h"
#include "origo/assets/Texture2D.h"

#include "origo/assets/material/Material.h"
#include "origo/assets/material/MaterialDeps.h"
#include "origo/assets/material/MaterialPBRSource.h"

namespace Origo {

class MaterialPBR : public Material {
public:
	MaterialPBR();

	void resolve() override;
	void bind() override;

	MaterialPBRData& get_data() { return m_data; }
	const MaterialPBRData& get_data() const { return m_data; }

	AssetType get_asset_type() const override { return AssetType::MaterialPBR; }
	static AssetType get_class_asset_type() { return AssetType::MaterialPBR; }

	MaterialPBR& set_shader(const OptionalAssetHandle& shader) {
		Material::set_shader(shader);
		return *this;
	}
	MaterialPBR& set_albedo(const OptionalAssetHandle& albedo);
	MaterialPBR& set_normal(const OptionalAssetHandle& normal);
	MaterialPBR& set_metallic_roughness(const OptionalAssetHandle& metallicRoughness);
	MaterialPBR& set_ao(const OptionalAssetHandle& ao);
	MaterialPBR& set_emissive(const OptionalAssetHandle& emissive);
	MaterialPBR& set_unlit(bool unlit);

private:
	void resolve_albedo();
	void resolve_normal();
	void resolve_metallic_roughness();
	void resolve_ao();
	void resolve_emissive();

private:
	MaterialPBRData m_data {};

	Texture2D* m_albedo { nullptr };
	Texture2D* m_normal { nullptr };
	Texture2D* m_metallic_roughness { nullptr };
	Texture2D* m_ao { nullptr };
	Texture2D* m_emissive { nullptr };
};

}
