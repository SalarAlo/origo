#pragma once

#include "origo/assets/AssetManager.h"
#include "origo/assets/UniformList.hpp"
#include "origo/assets/Shader.h"
#include <optional>

namespace Origo {

class Material2D : public Asset {
public:
	Material2D();
	Material2D(AssetHandle shader, OptionalAssetHandle texture = std::nullopt);
	Material2D(UUID shader, UUID material);

	static AssetHandle DefaultMaterial2D();

	void Resolve() override;
	OptionalAssetHandle GetShader() const { return m_Shader; }
	void SetShader(const AssetHandle& handle) { m_Shader = handle; }
	void SetShaderUUID(const UUID& uuid) { m_ShaderUUID = uuid; };

	UniformList& GetUniformList() { return m_UniformList; }

	OptionalAssetHandle GetAlbedo() const { return m_Albedo; }
	void SetAlbedoUUID(const UUID& uuid) { m_AlbedoUUID = uuid; };
	void SetAlbedo(const AssetHandle& handle) { m_Albedo = handle; }

	void Bind();
	void WriteModel(const glm::mat4& model);

	template <typename T>
	Material2D& SetShaderDirectly(std::string_view name, const T& val) {
		if (!m_Shader.has_value()) {
			ORG_CORE_TRACE("Material2D::SetShaderDirectly: Can not set value on shader if no asset handle");
			return *this;
		}

		auto shader { AssetManager::GetInstance().Get<Shader>(*m_Shader) };
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
	OptionalAssetHandle m_Shader { std::nullopt };
	OptionalAssetHandle m_Albedo { std::nullopt };

	OptionalUUID m_ShaderUUID { std::nullopt };
	OptionalUUID m_AlbedoUUID { std::nullopt };

	UniformList m_UniformList {};
};

}
