#pragma once

#include "origo/assets/Script.h"
#include "origo/assets/Shader.h"
#include "origo/assets/ShaderSource.h"

namespace OrigoEditor {
struct TextEditablePathVisitor {
	std::optional<std::filesystem::path> operator()(Origo::Script* script) const {
		return script->get_path();
	}

	std::optional<std::filesystem::path> operator()(Origo::Shader* shader) const {
		auto* source { shader->get_source() };
		auto* file_source { dynamic_cast<Origo::ShaderSourceFile*>(source) };

		if (!file_source)
			return std::nullopt;

		return file_source->get_path();
	}
};
}
