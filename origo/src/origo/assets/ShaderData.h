#pragma once

namespace Origo {

struct ShaderData {
	ShaderData() = default;
	ShaderData(const char* vertexShader, const char* fragmentShader)
	    : VertexShader(vertexShader)
	    , FragmentShader(fragmentShader) {
	}

	const char* VertexShader {};
	const char* FragmentShader {};
};

}
