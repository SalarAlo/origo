#pragma once

#include "origo/assets/ShaderSource.h"
#include <glm/glm.hpp>
#include "origo/assets/Asset.h"

namespace Origo {

class Shader : public Asset {
public:
	Shader() = default;
	~Shader();

	void UseProgram() const;

	template <typename T>
	void SetUniform(std::string_view name, const T& value) const;

	void SetSource(Scope<ShaderSource> source);
	ShaderSource* GetSource() const { return m_Source.get(); }

	AssetType GetAssetType() const override { return AssetType::Shader; };
	static AssetType GetClassAssetType() { return AssetType::Shader; }

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&&) = delete;
	Shader& operator=(Shader&&) = delete;

private:
	void Init();

private:
	Scope<ShaderSource> m_Source {};
	unsigned int m_ProgramId {};
};
}
