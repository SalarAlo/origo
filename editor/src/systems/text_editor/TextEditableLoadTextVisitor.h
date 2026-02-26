#pragma once

#include "origo/assets/Script.h"
#include "origo/assets/Shader.h"

#include "systems/text_editor/TextEditablePathVisitor.h"

namespace OrigoEditor {

struct TextEditableLoadTextVisitor {
	std::string operator()(Origo::Script* script) const {
		return load_from_file(script->get_path());
	}

	std::string operator()(Origo::Shader* shader) const {
		return load_from_file(*TextEditablePathVisitor {}(shader));
	}

private:
	static std::string load_from_file(const std::filesystem::path& path) {
		std::ifstream file(path);
		if (!file.is_open())
			return {};

		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}
};
}
