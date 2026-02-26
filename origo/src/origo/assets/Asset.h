#pragma once

namespace Origo {

enum class AssetType {
	Shader,
	Texture2D,
	TextureCubemap,
	Material2D,
	SkyboxMaterial,
	Mesh,
	Model,
	Script,
	Scene
};

class Asset {
public:
	virtual ~Asset() = default;
	virtual AssetType get_asset_type() const = 0;
	virtual void resolve() { };

protected:
	AssetType m_asset_type;
};

}
