#pragma once

#include "origo/assets/Shader.h"
#include "nlohmann/json.hpp"

namespace Origo {

enum class UniformType {
	Int,
	Float,
	Vec2,
	Vec3,
	Mat4
};

class UniformBase {
public:
	virtual void Upload(Ref<Shader> shader, std::string_view name) const = 0;
	virtual ~UniformBase() = default;

	virtual UniformType GetUniformType() const = 0;
	virtual nlohmann::json Serialize() const = 0;
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

	UniformType GetUniformType() const override;
	nlohmann::json Serialize() const override;

private:
	T m_Value;
};

class UniformList {
public:
	template <typename T>
	void AddUniform(const std::string& name, const T& value) {
		m_Uniforms[name] = std::make_unique<Uniform<T>>(value);
	}

	void UploadAll(Ref<Shader> shader) const {
		for (const auto& [name, uniform] : m_Uniforms) {
			uniform->Upload(shader, name);
		}
	}

	nlohmann::json Serialize() const {
		nlohmann::json j = nlohmann::json::array();

		for (const auto& [name, base] : m_Uniforms) {
			nlohmann::json uniformJ;

			uniformJ["name"] = name;
			uniformJ.merge_patch(base->Serialize());

			j.push_back(uniformJ);
		}

		return j;
	}

	const auto& GetUniforms() const { return m_Uniforms; }

private:
	std::unordered_map<std::string, std::unique_ptr<UniformBase>> m_Uniforms {};
};

}
