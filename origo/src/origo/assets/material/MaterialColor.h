#pragma once

#include "origo/assets/material/Material.h"

namespace Origo {

struct MaterialColorData {
	bool Unlit { false };
};

class MaterialColor : public Material {
public:
	MaterialColor();

	void resolve() override;
	void bind() override;

	MaterialColorData& get_data() { return m_data; }
	const MaterialColorData& get_data() const { return m_data; }

	AssetType get_asset_type() const override { return AssetType::MaterialColor; }
	static AssetType get_class_asset_type() { return AssetType::MaterialColor; }

	MaterialColor& set_shader(const OptionalAssetHandle& shader) {
		Material::set_shader(shader);
		return *this;
	}

	MaterialColor& set_unlit(bool unlit) {
		m_data.Unlit = unlit;
		return *this;
	}

private:
	MaterialColorData m_data {};
};

}
