#pragma once

#include "origo/assets/ShaderSource.h"
#include <glm/glm.hpp>
#include "origo/assets/Asset.h"

namespace Origo {

class Shader : public Asset {
public:
	Shader(std::string_view path);
	Shader(std::string_view vertexShader, std::string_view fragmentShader);
	~Shader();

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	Shader(Shader&&) = delete;
	Shader& operator=(Shader&&) = delete;

	void UseProgram() const;

	template <typename T>
	void SetUniform(std::string_view name, const T& value) const;

	Ref<ShaderSource> GetSource() const { return m_Source; }

	AssetType GetAssetType() const override {
		return AssetType::Shader;
	};

private:
	void Init();

private:
	Ref<ShaderSource> m_Source;
	unsigned int m_ProgramId {};
};
}
