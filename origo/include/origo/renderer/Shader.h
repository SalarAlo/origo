#pragma once

#include "origo/core/Identifiable.h"
#include "origo/renderer/ShaderSource.h"
#include <glm/glm.hpp>

namespace Origo {

class Shader : public Identifiable {
public:
	Shader(Ref<ShaderSource> src);

	~Shader();
	void UseProgram() const;

	template <typename T>
	void SetUniform(std::string_view name, const T& value) const;

	Ref<ShaderSource> GetSource() const { return m_Source; }

private:
	Ref<ShaderSource> m_Source;
	unsigned int m_ProgramId {};
};
}
