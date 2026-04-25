#pragma once

#include "origo/assets/ShaderData.h"

#include <filesystem>

#include "origo/serialization/ISerializer.h"

namespace Origo {

enum class ShaderSourceType {
	File,
	Raw,
};

class ShaderSource {
public:
	virtual ~ShaderSource() = default;
	virtual ShaderData get_shader_data() const = 0;
	virtual ShaderSourceType get_source_type() const = 0;

	void serialize(ISerializer& backend) const;
	static Scope<ShaderSource> deserialize(ISerializer& backend);

protected:
	virtual void serialize_body(ISerializer& backend) const = 0;
};

class ShaderSourceRaw : public ShaderSource {
public:
	ShaderSourceRaw(std::string_view vertex, std::string_view fragment)
	    : m_shader_data(vertex.data(), fragment.data()) {
	}

	ShaderData get_shader_data() const override {
		return m_shader_data;
	}

	ShaderSourceType get_source_type() const override {
		return ShaderSourceType::Raw;
	}

	void serialize_body(ISerializer& backend) const override {
		backend.write("vertex_shader", m_shader_data.VertexShader);
		backend.write("fragment_shader", m_shader_data.FragmentShader);
	}

private:
	ShaderData m_shader_data;
};

class ShaderSourceFile : public ShaderSource {
public:
	ShaderSourceFile(const std::filesystem::path& path)
	    : m_path(path) { };

	ShaderData get_shader_data() const override;
	const std::filesystem::path& get_path() const { return m_path; }

	ShaderSourceType get_source_type() const override {
		return ShaderSourceType::File;
	}

	void serialize_body(ISerializer& backend) const override;

private:
	std::filesystem::path m_path;
};

}
