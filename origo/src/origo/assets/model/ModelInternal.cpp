#include <cmath>

#include "ModelInternal.h"

namespace Origo::ModelInternal {

glm::mat4 convert_matrix(const aiMatrix4x4& matrix) {
	glm::mat4 out;
	out[0][0] = matrix.a1;
	out[1][0] = matrix.a2;
	out[2][0] = matrix.a3;
	out[3][0] = matrix.a4;
	out[0][1] = matrix.b1;
	out[1][1] = matrix.b2;
	out[2][1] = matrix.b3;
	out[3][1] = matrix.b4;
	out[0][2] = matrix.c1;
	out[1][2] = matrix.c2;
	out[2][2] = matrix.c3;
	out[3][2] = matrix.c4;
	out[0][3] = matrix.d1;
	out[1][3] = matrix.d2;
	out[2][3] = matrix.d3;
	out[3][3] = matrix.d4;
	return out;
}

void make_fallback_model(std::vector<Model::Node>& nodes, int& root_node) {
	nodes.clear();
	nodes.reserve(1);

	Model::Node& node = nodes.emplace_back();
	node.Parent = -1;
	node.SubMeshIndex = -1;
	node.LocalTransform = glm::mat4(1.0f);
	node.Children.clear();

	root_node = 0;
}

aiVector3D normalize_safe(const aiVector3D& value) {
	const float length_squared = value.x * value.x + value.y * value.y + value.z * value.z;
	if (length_squared <= 0.000001f)
		return aiVector3D(0.0f, 1.0f, 0.0f);

	const float inverse_length = 1.0f / std::sqrt(length_squared);
	return aiVector3D(value.x * inverse_length, value.y * inverse_length, value.z * inverse_length);
}

aiVector3D cross3(const aiVector3D& lhs, const aiVector3D& rhs) {
	return aiVector3D(
	    lhs.y * rhs.z - lhs.z * rhs.y,
	    lhs.z * rhs.x - lhs.x * rhs.z,
	    lhs.x * rhs.y - lhs.y * rhs.x);
}

float dot3(const aiVector3D& lhs, const aiVector3D& rhs) {
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

}
