#pragma once

#include <assimp/scene.h>

#include "Model.h"

namespace Origo::ModelInternal {

glm::mat4 convert_matrix(const aiMatrix4x4& matrix);

void make_fallback_model(std::vector<Model::Node>& nodes, int& root_node);

aiVector3D normalize_safe(const aiVector3D& value);
aiVector3D cross3(const aiVector3D& lhs, const aiVector3D& rhs);
float dot3(const aiVector3D& lhs, const aiVector3D& rhs);

}
