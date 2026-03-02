#pragma once

#include <optional>

#include "origo/assets/AssetManager.h"
#include "origo/assets/Material2DSource.h"
#include "origo/assets/Shader.h"
#include "origo/assets/Texture2D.h"
#include "origo/assets/UniformList.hpp"

namespace Origo {

class Material2D : public Asset {
public:
	Material2D() = default;

	void resolve() override;
	void bind();
	void write_model(const glm::mat4& model);
	void make_textured_material();

	void set_color(Vec3 color);
	Vec3 get_color() const { return m_color; }

	const UniformList& get_uniform_list() const { return m_uniform_list; }
	UniformList& get_uniform_list() { return m_uniform_list; }

	Material2DSource* get_source() const;
	void set_source(Scope<Material2DSource> source);
	std::optional<MaterialData> get_data() const;

	AssetType get_asset_type() const override { return AssetType::Material2D; }
	static AssetType get_class_asset_type() { return AssetType::Material2D; }

	template <typename T>
	Material2D& set_shader_directly(std::string_view name, const T& val) {
		if (!m_resolved) {
			ORG_CORE_TRACE("Material2D::SetShaderDirectly: Can not set value on shader if no asset handle");
			return *this;
		}

		auto shader { AssetManager::get_instance().get_asset<Shader>(m_data->ShaderHandle) };
		shader->set_uniform(name, val);
		return *this;
	}

	template <typename T>
	Material2D& set_uniform(std::string_view name, const T& val) {
		m_uniform_list.add_uniform<T>(name.data(), val);
		return *this;
	}

private:
	Scope<Material2DSource> m_source { nullptr };
	std::optional<MaterialData> m_data {};
	bool m_resolved { false };

	Shader* m_shader { nullptr };
	Texture2D* m_albedo { nullptr };

	Vec3 m_color { 1.0f };

	UniformList m_uniform_list {};
};

}
