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
	virtual AssetType GetAssetType() const = 0;
	virtual void Resolve() { };

protected:
	AssetType m_AssetType;
};

}
