#pragma once

#include "origo/assets/AssetManager.h"
#include "origo/assets/Shader.h"
#include "origo/assets/UniformList.hpp"

#include "origo/assets/material/MaterialDeps.h"

namespace Origo {

class Material : public Asset {
public:
	virtual ~Material() = default;

	virtual void bind() = 0;
	void write_model(const glm::mat4& modelMatrix);

	MaterialDeps& get_deps() { return m_uuid_deps; }
	const MaterialDeps& get_deps() const { return m_uuid_deps; }

	const UniformList& get_uniform_list() const { return m_uniform_list; }
	UniformList& get_uniform_list() { return m_uniform_list; }

	template <typename T>
	Material& set_uniform_directly(std::string_view name, const T& val) {
		if (!m_shader)
			return *this;

		m_shader->set_uniform(name, val);
		return *this;
	}

	template <typename T>
	Material& set_uniform(std::string_view name, const T& val) {
		m_uniform_list.add_uniform<T>(name.data(), val);
		return *this;
	}

	Material& set_shader(const OptionalAssetHandle& shader);
	const OptionalAssetHandle& get_shader_handle() const { return m_shader_handle; }
	Shader* get_shader() const { return m_shader; }

	static AssetType get_class_asset_type() { return AssetType::Material; }

protected:
	explicit Material(OptionalAssetHandle defaultShader = std::nullopt)
	    : m_shader_handle(defaultShader) {
	}

	void bind_shader_and_upload_uniforms();
	void resolve_material_base();
	void resolve_shader();

protected:
	OptionalAssetHandle m_shader_handle;
	MaterialDeps m_uuid_deps;

	Shader* m_shader { nullptr };
	UniformList m_uniform_list {};
};

}
