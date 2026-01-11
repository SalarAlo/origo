#pragma once

#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/ShaderSource.h"
#include "origo/assets/Asset.h"

namespace Origo {

class Shader : public Asset {
public:
	Shader() = default;
	~Shader();

	static AssetHandle DefaultShader();
	static void CreateDefaultShader();
	void UseProgram() const;

	template <typename T>
	void SetUniform(std::string_view name, const T& value) const;

	void SetSource(Scope<ShaderSource> source);
	ShaderSource* GetSource() const { return m_Source.get(); }

	AssetType GetAssetType() const override { return AssetType::Shader; };
	static AssetType GetClassAssetType() { return AssetType::Shader; }

	GLuint GetProgramID() const { return m_ProgramId; }

	void Resolve() override {
		Init();
	}

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&&) = delete;
	Shader& operator=(Shader&&) = delete;

private:
	void Init();
	GLint GetUniformLocation(std::string_view name) const;

private:
	Scope<ShaderSource> m_Source {};

	unsigned int m_ProgramId {};
	mutable std::unordered_map<std::string, GLint> m_UniformCache;
};
}
