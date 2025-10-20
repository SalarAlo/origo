#pragma once

#include "nlohmann/json.hpp"
#include "origo/assets/ShaderData.h"

namespace Origo {

class ShaderSource {
public:
	virtual ~ShaderSource() = default;
	virtual ShaderData GetShaderData() const = 0;
	virtual std::string GetTypeName() const = 0;
};

class ShaderSourceRaw : public ShaderSource {
public:
	ShaderSourceRaw(std::string_view vertex, std::string_view fragment)
	    : m_ShaderData(vertex.data(), fragment.data()) {
	}

	ShaderData GetShaderData() const override {
		return m_ShaderData;
	}

	std::string GetTypeName() const override {
		return GetTypeNameClass();
	}
	static std::string GetTypeNameClass() {
		return "ShaderSourceRaw";
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

	std::string GetTypeName() const override {
		return GetTypeNameClass();
	}
	static std::string GetTypeNameClass() {
		return "ShaderSourceFile";
	}

private:
	std::string m_Path;
};

}
