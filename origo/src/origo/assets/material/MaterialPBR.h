#pragma once

#include "origo/assets/AssetManager.h"
#include "origo/assets/Shader.h"
#include "origo/assets/Texture2D.h"
#include "origo/assets/UniformList.hpp"

#include "origo/assets/material/MaterialDeps.h"
#include "origo/assets/material/MaterialPBRSource.h"

namespace Origo {

class MaterialPBR : public Asset {
public:
	MaterialPBR() = default;

	void resolve() override;
	void bind();

	void write_model(const glm::mat4& model);

	MaterialDeps& get_deps() { return m_uuid_deps; };

	const UniformList& get_uniform_list() const { return m_uniform_list; }
	UniformList& get_uniform_list() { return m_uniform_list; }

	MaterialData& get_data() { return m_data; }
	const MaterialData& get_data() const { return m_data; }

	AssetType get_asset_type() const override { return AssetType::MaterialPBR; }
	static AssetType get_class_asset_type() { return AssetType::MaterialPBR; }

	template <typename T>
	MaterialPBR& set_uniform_directly(std::string_view name, const T& val) {
		if (!m_shader)
			return *this;

		m_shader->set_uniform(name, val);
		return *this;
	}

	MaterialPBR& set_shader(const OptionalAssetHandle& shader);
	MaterialPBR& set_albedo(const OptionalAssetHandle& albedo);
	MaterialPBR& set_normal(const OptionalAssetHandle& normal);
	MaterialPBR& set_metallic_roughness(const OptionalAssetHandle& metallicRoughness);
	MaterialPBR& set_ao(const OptionalAssetHandle& ao);
	MaterialPBR& set_emissive(const OptionalAssetHandle& emissive);
	MaterialPBR& set_unlit(bool unlit);

	template <typename T>
	MaterialPBR& set_uniform(std::string_view name, const T& val) {
		m_uniform_list.add_uniform<T>(name.data(), val);
		return *this;
	}

private:
	void resolve_shader();
	void resolve_albedo();
	void resolve_normal();
	void resolve_metallic_roughness();
	void resolve_ao();
	void resolve_emissive();

private:
	MaterialData m_data {};
	MaterialDeps m_uuid_deps;

	Shader* m_shader { nullptr };
	Texture2D* m_albedo { nullptr };
	Texture2D* m_normal { nullptr };
	Texture2D* m_metallic_roughness { nullptr };
	Texture2D* m_ao { nullptr };
	Texture2D* m_emissive { nullptr };

	UniformList m_uniform_list {};
};

}
