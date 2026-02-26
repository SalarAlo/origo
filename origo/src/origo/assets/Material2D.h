#pragma once

#include <optional>

#include "origo/assets/AssetManager.h"
#include "origo/assets/Shader.h"
#include "origo/assets/UniformList.hpp"

namespace Origo {

class Material2D : public Asset {
public:
	Material2D();
	Material2D(AssetHandle shader, OptionalAssetHandle texture = std::nullopt);
	Material2D(UUID shader, UUID material);

	void resolve() override;

	void set_color(Vec3 color);
	Vec3 get_color() const { return m_color; }

	OptionalAssetHandle get_shader() const { return m_shader; }
	void set_shader(const AssetHandle& handle) { m_shader = handle; }
	void set_shader_uuid(const UUID& uuid) { m_shader_uuid = uuid; };

	UniformList& get_uniform_list() { return m_uniform_list; }

	OptionalAssetHandle get_albedo() const { return m_albedo; }
	void set_albedo_uuid(const UUID& uuid) { m_albedo_uuid = uuid; };
	void set_albedo(const AssetHandle& handle) { m_albedo = handle; }

	void bind();
	void write_model(const glm::mat4& model);

	template <typename T>
	Material2D& set_shader_directly(std::string_view name, const T& val) {
		if (!m_shader.has_value()) {
			ORG_CORE_TRACE("Material2D::SetShaderDirectly: Can not set value on shader if no asset handle");
			return *this;
		}

		auto shader { AssetManager::get_instance().get_asset<Shader>(*m_shader) };
		shader->set_uniform(name, val);
		return *this;
	}

	template <typename T>
	Material2D& set_uniform(std::string_view name, const T& val) {
		m_uniform_list.add_uniform<T>(name.data(), val);
		return *this;
	}

	AssetType get_asset_type() const override { return AssetType::Material2D; }
	static AssetType get_class_asset_type() { return AssetType::Material2D; }

private:
	OptionalAssetHandle m_shader { std::nullopt };
	OptionalAssetHandle m_albedo { std::nullopt };

	OptionalUUID m_shader_uuid { std::nullopt };
	OptionalUUID m_albedo_uuid { std::nullopt };

	Vec3 m_color { 1.0f };

	UniformList m_uniform_list {};
};

}
