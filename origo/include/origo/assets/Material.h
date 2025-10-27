#pragma once

#include "origo/assets/Shader.h"
#include "origo/assets/Texture.h"
#include "origo/assets/UniformList.hpp"

namespace Origo {

class Material : public Asset {
public:
	Material(const Ref<Shader>& shader, const Ref<Texture>& material = nullptr, bool shouldSerialize = true);

	Ref<Shader> GetShader() { return m_Shader; }
	Ref<UniformList> GetUniformList() { return m_UniformList; }
	Ref<Texture> GetAlbedo() { return m_Albedo; }

	void Bind();
	void WriteModel(const glm::mat4& model);

	template <typename T>
	Material& SetShaderDirectly(std::string_view name, const T& val) {
		m_Shader->SetUniform(name, val);
		return *this;
	}

	template <typename T>
	Material& SetUniform(std::string_view name, const T& val) {
		m_UniformList->AddUniform<T>(name.data(), val);
		return *this;
	}

	AssetType GetAssetType() const override {
		return AssetType::Material;
	}

	bool ShouldSerialize() const override {
		return m_ShouldSerialize;
	}

private:
	Ref<Shader> m_Shader {};
	Ref<UniformList> m_UniformList {};
	Ref<Texture> m_Albedo {};

	bool m_ShouldSerialize;
};

}
