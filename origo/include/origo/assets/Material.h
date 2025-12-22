#pragma once

#include "origo/assets/Shader.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/UniformList.hpp"

namespace Origo {

class Material : public Asset {
public:
	Material() = default;
	Material(AssetHandle shader, AssetHandle texture = {});
	Material(UUID shader, UUID material);

	void Resolve() override {
		auto& am = AssetManagerFast::GetInstance();

		m_Shader = am.Resolve(m_ShaderUUID);
		m_Albedo = am.Resolve(m_AlbedoUUID);
	}

	AssetHandle GetShader() const { return m_Shader; }
	UniformList& GetUniformList() { return m_UniformList; }
	AssetHandle GetAlbedo() const { return m_Albedo; }

	void Bind();
	void WriteModel(const glm::mat4& model);

	template <typename T>
	Material& SetShaderDirectly(std::string_view name, const T& val) {
		auto shader { AssetManagerFast::GetInstance().Get<Shader>(m_Shader) };
		shader->SetUniform(name, val);
		return *this;
	}

	template <typename T>
	Material& SetUniform(std::string_view name, const T& val) {
		m_UniformList.AddUniform<T>(name.data(), val);
		return *this;
	}

	AssetType GetAssetType() const override { return AssetType::Material; }
	static AssetType GetClassAssetType() { return AssetType::Material; }

private:
	AssetHandle m_Shader {};
	AssetHandle m_Albedo {};

	UUID m_ShaderUUID {};
	UUID m_AlbedoUUID {};

	UniformList m_UniformList {};
};

}
