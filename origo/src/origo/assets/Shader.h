#pragma once

#include "origo/assets/Asset.h"
#include "origo/assets/ShaderSource.h"

namespace Origo {

class Shader : public Asset {
public:
	Shader() = default;
	~Shader();

	void use_program() const;
	void unuse_program() const;

	template <typename T>
	void set_uniform(std::string_view name, const T& value) const;

	void set_source(Scope<ShaderSource> source);
	ShaderSource* get_source() const { return m_source.get(); }

	AssetType get_asset_type() const override { return AssetType::Shader; };
	static AssetType get_class_asset_type() { return AssetType::Shader; }

	GLuint get_program_id() const { return m_program_id; }

	void resolve() override;

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&&) = delete;
	Shader& operator=(Shader&&) = delete;

private:
	GLint get_uniform_location(std::string_view name) const;

private:
	Scope<ShaderSource> m_source {};

	unsigned int m_program_id {};
	mutable std::unordered_map<std::string, GLint> m_uniform_cache;
};
}
