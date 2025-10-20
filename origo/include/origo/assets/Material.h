#pragma once

#include "origo/assets/Mesh.h"
#include "origo/core/Identifiable.h"
#include "origo/assets/Shader.h"
#include "origo/assets/Texture.h"
#include "origo/assets/UniformList.hpp"

namespace Origo {

class Material : public Asset {
public:
	Material(const Ref<Shader>& shader, const Ref<Texture>& material = nullptr);

	void Bind();
	void WriteModel(const glm::mat4& model);

	template <typename T>
	Material& Set(std::string_view name, const T& val) {
		m_Shader->SetUniform(name, val);
		return *this;
	}

	AssetType GetAssetType() const {
		return AssetType::Material;
	}

private:
	Ref<Shader> m_Shader {};
	Ref<UniformList> m_UniformList {};
	Ref<Texture> m_Albedo {};
};

}
