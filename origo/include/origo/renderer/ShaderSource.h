#pragma once

namespace Origo {

struct ShaderData {
	const char* VertexShader {};
	const char* FragmentShader {};
};

class ShaderSource {
public:
	virtual ~ShaderSource() = default;
	virtual ShaderData GetShaderData() const = 0;
};

class ShaderSourceRaw : public ShaderSource {
public:
	ShaderSourceRaw(std::string_view vertex, std::string_view fragment)
	    : m_ShaderData(vertex.data(), fragment.data()) {
	}

	ShaderData GetShaderData() const override {
		return m_ShaderData;
	}

private:
	ShaderData m_ShaderData;
};

class ShaderSourceFile : public ShaderSource {
public:
	ShaderSourceFile(std::string_view path)
	    : m_Path(path) { };

	ShaderData GetShaderData() const override;

private:
	std::string m_Path;
};

}
