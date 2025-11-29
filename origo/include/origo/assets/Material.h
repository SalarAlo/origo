#pragma once

#include "origo/assets/Shader.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/UniformList.hpp"

namespace Origo {

class Material : public Asset {
public:
	Material(RID shader, RID material);

	RID GetShader() const { return m_Shader; }
	UniformList& GetUniformList() { return m_UniformList; }
	RID GetAlbedo() const { return m_Albedo; }

	void Bind();
	void WriteModel(const glm::mat4& model);

	template <typename T>
	Material& SetShaderDirectly(std::string_view name, const T& val) {
		auto shader { AssetManager::GetAssetAs<Shader>(m_Shader) };
		shader->SetUniform(name, val);
		return *this;
	}

	template <typename T>
	Material& SetUniform(std::string_view name, const T& val) {
		m_UniformList.AddUniform<T>(name.data(), val);
		return *this;
	}

	AssetType GetAssetType() const override { return AssetType::Material; }

private:
	RID m_Shader {};
	RID m_Albedo {};
	UniformList m_UniformList {};
};

}
