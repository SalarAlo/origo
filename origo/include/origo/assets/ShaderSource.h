#pragma once

#include "origo/assets/ShaderData.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

enum class ShaderSourceType {
	File,
	Raw,
};

class ShaderSource {
public:
	virtual ~ShaderSource() = default;
	virtual ShaderData GetShaderData() const = 0;
	virtual ShaderSourceType GetSourceType() const = 0;

	void Serialize(ISerializer& backend) const;
	static Scope<ShaderSource> Deserialize(ISerializer& backend);

protected:
	virtual void SerializeBody(ISerializer& backend) const = 0;
};

class ShaderSourceRaw : public ShaderSource {
public:
	ShaderSourceRaw(std::string_view vertex, std::string_view fragment)
	    : m_ShaderData(vertex.data(), fragment.data()) {
	}

	ShaderData GetShaderData() const override {
		return m_ShaderData;
	}

	ShaderSourceType GetSourceType() const override {
		return ShaderSourceType::Raw;
	}

	void SerializeBody(ISerializer& backend) const override {
		backend.Write("vertex_shader", m_ShaderData.VertexShader);
		backend.Write("fragment_shader", m_ShaderData.FragmentShader);
	}

private:
	ShaderData m_ShaderData;
};

class ShaderSourceFile : public ShaderSource {
public:
	ShaderSourceFile(std::string_view path)
	    : m_Path(path) { };

	ShaderData GetShaderData() const override;
	std::string GetPath() const { return m_Path; }

	ShaderSourceType GetSourceType() const override {
		return ShaderSourceType::File;
	}

	void SerializeBody(ISerializer& backend) const override {
		backend.Write("shader_path", m_Path);
	}

private:
	std::string m_Path;
};

}
