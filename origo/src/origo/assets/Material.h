#pragma once

#include "origo/assets/Shader.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/UniformList.hpp"

namespace Origo {

class Material2D : public Asset {
public:
	Material2D();
	Material2D(AssetHandle shader, AssetHandle texture = {});
	Material2D(UUID shader, UUID material);

	static AssetHandle DefaultMaterial2D();

	void Resolve() override;
	AssetHandle GetShader() const { return m_Shader; }
	void SetShader(const AssetHandle& handle) { m_Shader = handle; }
	void SetShaderUUID(const UUID& uuid) { m_ShaderUUID = uuid; };

	UniformList& GetUniformList() { return m_UniformList; }

	AssetHandle GetAlbedo() const { return m_Albedo; }
	void SetAlbedoUUID(const UUID& uuid) { m_AlbedoUUID = uuid; };
	void SetAlbedo(const AssetHandle& handle) { m_Albedo = handle; }

	void Bind();
	void WriteModel(const glm::mat4& model);

	template <typename T>
	Material2D& SetShaderDirectly(std::string_view name, const T& val) {
		auto shader { AssetManager::GetInstance().Get<Shader>(m_Shader) };
		shader->SetUniform(name, val);
		return *this;
	}

	template <typename T>
	Material2D& SetUniform(std::string_view name, const T& val) {
		m_UniformList.AddUniform<T>(name.data(), val);
		return *this;
	}

	AssetType GetAssetType() const override { return AssetType::Material2D; }
	static AssetType GetClassAssetType() { return AssetType::Material2D; }

private:
	AssetHandle m_Shader {};
	AssetHandle m_Albedo {};

	UUID m_ShaderUUID {};
	UUID m_AlbedoUUID {};

	UniformList m_UniformList {};
};

}
