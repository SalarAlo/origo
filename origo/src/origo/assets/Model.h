#pragma once

#include <filesystem>
#include <vector>

#include <glm/glm.hpp>

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManagerFast.h"

struct aiNode;

namespace Origo {

struct Model : public Asset {
public:
	struct SubMesh {
		AssetHandle Mesh;
		AssetHandle Material;
	};

	struct Node {
		glm::mat4 LocalTransform { 1.0f };
		int Parent = -1;
		int SubMeshIndex = -1;
		std::vector<int> Children;
	};

public:
	Model() = default;
	Model(const std::filesystem::path& path, const AssetHandle& shader);

	void Load();

	AssetType GetAssetType() const override;
	static AssetType GetClassAssetType();

	const std::filesystem::path& GetPath() const;
	int GetRootNode() const;

	const std::vector<Node>& GetNodes() const;
	const std::vector<SubMesh>& GetSubMeshes() const;

	AssetHandle GetShaderHandle() const;
	void SetShaderHandle(const AssetHandle&);

	void SetPath(const std::filesystem::path& path);

private:
	void LoadFromAssimp();
	void Clear();

	int ProcessNode(struct aiNode* node, int parent);

private:
	std::filesystem::path m_Path {};

	int m_RootNode = -1;
	std::vector<SubMesh> m_SubMeshes;
	std::vector<Node> m_Nodes;
	AssetHandle m_ModelShader;
};

}
