#pragma once

#include "origo/assets/Shader.h"

namespace Origo {

class UniformBase {
public:
	virtual void Upload(Ref<Shader> shader, std::string_view name) const = 0;
	virtual ~UniformBase() = default;
};

template <typename T>
class Uniform : public UniformBase {
public:
	explicit Uniform(const T& value)
	    : m_Value(value) { }

	void Upload(Ref<Shader> shader, std::string_view name) const override {
		shader->SetUniform<T>(name, m_Value);
	}

	const T& GetValue() const { return m_Value; }

private:
	T m_Value;
};

class UniformList {
public:
	template <typename T>
	void AddUniform(const std::string& name, const T& value) {
		uniforms[name] = std::make_unique<Uniform<T>>(value);
	}

	void UploadAll(Ref<Shader> shader) const {
		for (const auto& [name, uniform] : uniforms) {
			uniform->Upload(shader, name);
		}
	}

	const auto& GetUniforms() const { return uniforms; }

private:
	std::unordered_map<std::string, std::unique_ptr<UniformBase>> uniforms {};
};

}
