#pragma once

#include "origo/assets/Shader.h"

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
		m_uniforms[name] = MakeScope<Uniform<T>>(value);
	}

	void upload_all(Shader* shader) const {
		for (const auto& [name, uniform] : m_uniforms) {
			uniform->upload(shader, name);
		}
	}

	void serialize(ISerializer& backend) const {
		for (const auto& [name, base] : m_uniforms) {
			backend.begin_array_element();
			backend.write("name", name);
			base->serialize(backend);

			backend.end_array_element();
		}
	}

	void deserialize(ISerializer& backend) {
		m_uniforms.clear();

		while (backend.try_begin_array_element_read()) {
			std::string name;
			if (!backend.try_read("name", name))
				continue;

			auto uniform = UniformBase::deserialize(backend);
			if (!uniform)
				continue;

			m_uniforms[name] = std::move(uniform);
		}
	}

	const auto& get_uniforms() const { return m_uniforms; }

private:
	std::unordered_map<std::string, Scope<UniformBase>> m_uniforms {};
};

}
