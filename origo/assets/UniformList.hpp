#pragma once

#include <algorithm>
#include <vector>

#include "origo/assets/Shader.h"

#include "origo/core/Logger.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {

enum class UniformType {
	Int,
	UnsignedInt,
	Float,
	Vec2,
	Vec3,
	Mat4,
	Bool
};

class UniformBase {
public:
	static Scope<UniformBase> deserialize(ISerializer& backend);
	virtual void upload(Shader* shader, std::string_view name) const = 0;
	virtual ~UniformBase() = default;

	virtual UniformType get_uniform_type() const = 0;
	virtual void serialize(ISerializer& backend) const = 0;
};

template <typename T>
class Uniform : public UniformBase {
public:
	explicit Uniform(const T& value)
	    : m_value(value) { }

	void upload(Shader* shader, std::string_view name) const override {
		shader->set_uniform<T>(name, m_value);
	}

	const T& get_value() const { return m_value; }

	UniformType get_uniform_type() const override;
	void serialize(ISerializer& backend) const override;

private:
	T m_value;
};

class UniformList {
public:
	UniformList() = default;

	UniformList(const UniformList&) = delete;
	UniformList& operator=(const UniformList&) = delete;

	UniformList(UniformList&&) = default;
	UniformList& operator=(UniformList&&) = default;

	template <typename T>
	void add_uniform(const std::string& name, const T& value) {
		m_uniforms[name] = make_scope<Uniform<T>>(value);
	}

	void upload_all(Shader* shader) const {
		for (const auto& [name, uniform] : m_uniforms) {
			uniform->upload(shader, name);
		}
	}

	void serialize(ISerializer& backend) const {
		backend.begin_array("uniforms");

		std::vector<std::string> names;
		names.reserve(m_uniforms.size());
		for (const auto& [name, _] : m_uniforms)
			names.push_back(name);

		std::sort(names.begin(), names.end());

		for (const auto& name : names) {
			backend.begin_array_object();
			backend.write("name", name);
			m_uniforms.at(name)->serialize(backend);

			backend.end_array_object();
		}

		backend.end_array();
	}

	void deserialize(ISerializer& backend) {
		m_uniforms.clear();

		backend.begin_array("uniforms");

		while (backend.try_begin_array_object_read()) {
			std::string name;
			if (!backend.try_read("name", name)) {
				ORG_WARN("Skipping uniform entry without a valid 'name'");
				backend.end_array_object();
				continue;
			}

			auto uniform = UniformBase::deserialize(backend);
			if (!uniform) {
				ORG_WARN("Skipping uniform '{}' because it failed to deserialize", name);
				backend.end_array_object();
				continue;
			}

			m_uniforms[name] = std::move(uniform);

			backend.end_array_object();
		}

		backend.end_array();
	}

	const auto& get_uniforms() const { return m_uniforms; }

private:
	std::unordered_map<std::string, Scope<UniformBase>> m_uniforms {};
};

}
