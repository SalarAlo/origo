#pragma once

#include "origo/renderer/Shader.h"
#include "origo/renderer/Texture.h"
#include "origo/renderer/UniformList.hpp"

namespace Origo {

class Material {
public:
	Material(const Ref<Shader>& shader, const Ref<Texture>& material = nullptr);
	void Bind();
	void WriteModel(const glm::mat4& model);

	template <typename T>
	Material& Set(std::string_view name, const T& val) {
		m_Shader->SetUniform(name, val);
		return *this;
	}

private:
	Ref<Shader> m_Shader {};
	Ref<UniformList> m_UniformList {};
	Ref<Texture> m_Albedo {};
};

}
