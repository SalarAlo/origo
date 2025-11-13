#pragma once

#include "origo/core/Identifiable.h"

namespace Origo {

enum class AssetType {
	Shader,
	Texture,
	Material,
	Mesh,
	Model
};

class Asset : public Identifiable {
public:
	virtual ~Asset() = default;
	virtual AssetType GetAssetType() const = 0;

protected:
	AssetType m_AssetType;
};

}
