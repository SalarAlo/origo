#include "Model.h"

#include "ModelInternal.h"

namespace Origo {

int Model::process_node(aiNode* node, int parent) {
	const int node_index = (int)m_nodes.size();

	Node& destination = m_nodes.emplace_back();
	destination.Parent = parent;
	destination.LocalTransform = ModelInternal::convert_matrix(node->mTransformation);

	destination.SubMeshIndex = -1;
	for (uint32_t mesh_slot = 0; mesh_slot < node->mNumMeshes; ++mesh_slot) {
		const int assimp_mesh_index = (int)node->mMeshes[mesh_slot];
		if (assimp_mesh_index >= 0 && assimp_mesh_index < (int)m_assimp_mesh_to_sub_mesh.size()) {
			const int mapped_sub_mesh = m_assimp_mesh_to_sub_mesh[assimp_mesh_index];
			if (mapped_sub_mesh != -1) {
				destination.SubMeshIndex = mapped_sub_mesh;
				break;
			}
		}
	}

	destination.Children.reserve(node->mNumChildren);
	for (uint32_t child_index = 0; child_index < node->mNumChildren; ++child_index) {
		destination.Children.push_back(process_node(node->mChildren[child_index], node_index));
	}

	return node_index;
}

}

