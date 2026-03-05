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
	void make_textured_material();

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

	template <typename T>
	MaterialPBR& set_uniform(std::string_view name, const T& val) {
		m_uniform_list.add_uniform<T>(name.data(), val);
		return *this;
	}

private:
	void resolve_shader();
	void resolve_albedo();

private:
	MaterialData m_data {};
	MaterialDeps m_uuid_deps;

	Shader* m_shader { nullptr };
	Texture2D* m_albedo { nullptr };

	UniformList m_uniform_list {};
};

}
