#pragma once

namespace Origo {

enum class AssetType {
	Shader,
	Texture2D,
	TextureCubemap,
	MaterialPBR,
	SkyboxMaterial,
	Mesh,
	Model,
	Script,
	Scene,
	Audio,
	Prefab
};

class Asset {
public:
	virtual ~Asset() = default;
	virtual AssetType get_asset_type() const = 0;
	virtual void resolve() { };

	bool has_owner() const { return static_cast<bool>(m_owner); };
	Asset* get_owner() const { return m_owner; };
	void set_owner(Asset* owner) { m_owner = owner; };

protected:
	Asset* m_owner { nullptr };
};

}
