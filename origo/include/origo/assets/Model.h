#pragma once

#include "origo/assets/Asset.h"
#include "origo/assets/Material.h"
#include "origo/assets/Mesh.h"

namespace Origo {

struct SubMesh {
	Mesh* mesh;
	Material* material;
};

class Model : public Asset {
public:
	Model(std::string_view path, Shader* shader);

	std::string GetPath() const { return m_Path; }
	AssetType GetAssetType() const override { return AssetType::Model; }

	const std::vector<SubMesh>& GetSubMeshes() const { return m_SubMeshes; }

	void Render() const;

private:
	std::vector<SubMesh> m_SubMeshes;
	std::string m_Path;
};

}
