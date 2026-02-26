#pragma once

#include "origo/assets/Script.h"
#include "origo/assets/Shader.h"

#include "origo/scripting/ScriptSystem.h"

#include "systems/text_editor/TextEditablePathVisitor.h"

namespace OrigoEditor {

struct TextEditableSaveTextVisitor {
	std::string Text;

	void operator()(Origo::Script* script) const {
		std::ofstream out(script->get_path());
		out << Text;
		Origo::ScriptSystem::register_script(script->get_id(), script->get_path(), Text);
		Origo::ScriptSystem::reload_all_necessary();
	}

	void operator()(Origo::Shader* shader) const {
		auto path_opt = TextEditablePathVisitor {}(shader);

		if (!path_opt) {
			ORG_ERROR("Shader has no editable path");
			return;
		}

		{
			std::ofstream out(*path_opt, std::ios::binary);
			if (!out.is_open()) {
				ORG_ERROR("Failed to open shader file: {}", path_opt->string());
				return;
			}

			out << Text;
			out.flush();
		}

		shader->unuse_program();
		shader->resolve();
	}
};
}
