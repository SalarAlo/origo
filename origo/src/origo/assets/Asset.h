#pragma once

namespace Origo {

enum class AssetType {
	Shader,
	Texture2D,
	TextureCubemap,
	Material,
	MaterialPBR,
	MaterialColor,
	SkyboxMaterial,
	Mesh,
	Model,
	Script,
	Scene,
	Audio,
	Prefab
};

inline bool is_asset_type_compatible(AssetType actual, AssetType requested) {
	if (actual == requested)
		return true;

	switch (requested) {
	case AssetType::Material:
		return actual == AssetType::MaterialPBR || actual == AssetType::MaterialColor;
	default:
		return false;
	}
}

class Asset {
public:
	virtual ~Asset() = default;
	virtual AssetType get_asset_type() const = 0;
	virtual void resolve() { };
	virtual bool is_asset_type(AssetType type) const { return is_asset_type_compatible(get_asset_type(), type); }

	bool has_owner() const { return static_cast<bool>(m_owner); };
	Asset* get_owner() const { return m_owner; };
	void set_owner(Asset* owner) { m_owner = owner; };

protected:
	Asset* m_owner { nullptr };
};

}
