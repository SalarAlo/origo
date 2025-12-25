#pragma once

namespace Origo {

enum class AssetType {
	Shader,
	Texture2D,
	TextureCubemap,
	Material,
	SkyboxMaterial,
	Mesh,
	Model
};

class Asset {
public:
	virtual ~Asset() = default;
	virtual AssetType GetAssetType() const = 0;
	virtual void Resolve() { };

protected:
	AssetType m_AssetType;
};

}
