#pragma once

namespace Origo {

enum class AssetType {
	Shader,
	Texture,
	Material,
	Mesh,
	Model
};

class Asset {
public:
	virtual ~Asset() = default;
	virtual AssetType GetAssetType() const = 0;

protected:
	AssetType m_AssetType;
};

}
