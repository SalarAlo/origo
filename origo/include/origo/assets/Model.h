#pragma once

#include "origo/assets/Asset.h"
namespace Origo {

struct SubMesh {
	RID mesh;
	RID material;
};

class Model : public Asset {
public:
	Model(std::string_view path, RID shader);

	void Render() const;

	std::string GetPath() const { return m_Path; }
	const std::vector<SubMesh>& GetSubMeshes() const { return m_SubMeshes; }

	AssetType GetAssetType() const override { return AssetType::Model; }
	static AssetType GetClassAssetType() { return AssetType::Mesh; }

private:
	std::vector<SubMesh> m_SubMeshes;
	std::string m_Path;
};

}
