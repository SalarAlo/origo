#pragma once

#include <glm/glm.hpp>

#include <filesystem>
#include <vector>

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"

struct aiNode;

namespace Origo {

class Model : public Asset {
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
	Model();
	Model(const std::filesystem::path& path, const AssetHandle& shader);

	void load();

	AssetType get_asset_type() const override;
	static AssetType get_class_asset_type();

	const std::filesystem::path& get_path() const;
	int get_root_node() const;

	const std::vector<Node>& get_nodes() const;
	const std::vector<SubMesh>& get_sub_meshes() const;

	OptionalAssetHandle get_shader_handle() const;
	void set_shader_handle(const AssetHandle&);
	void set_shader_uuid(const UUID&);

	void set_path(const std::filesystem::path& path);

	void resolve() override;
	void ensure_shader();

private:
	void load_from_assimp();
	void clear();

	int process_node(struct aiNode* node, int parent);

private:
	std::filesystem::path m_path {};

	int m_root_node { -1 };

	std::vector<SubMesh> m_sub_meshes {};
	std::vector<Node> m_nodes {};
	std::vector<int> m_assimp_mesh_to_sub_mesh {};

	OptionalAssetHandle m_model_shader_handle { std::nullopt };
	OptionalUUID m_shader_uuid { std::nullopt };
};

}
