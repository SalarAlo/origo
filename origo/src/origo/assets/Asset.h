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
	MaterialPbr
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
