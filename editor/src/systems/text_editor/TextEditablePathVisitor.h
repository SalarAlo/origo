#pragma once

#include "origo/assets/Script.h"
#include "origo/assets/Shader.h"
#include "origo/assets/ShaderSource.h"

namespace OrigoEditor {
struct TextEditablePathVisitor {
	std::optional<std::filesystem::path> operator()(Origo::Script* script) const {
		return script->GetPath();
	}

	std::optional<std::filesystem::path> operator()(Origo::Shader* shader) const {
		auto* source { shader->GetSource() };
		auto* fileSource { dynamic_cast<Origo::ShaderSourceFile*>(source) };

		if (!fileSource)
			return std::nullopt;

		return fileSource->GetPath();
	}
};
}
