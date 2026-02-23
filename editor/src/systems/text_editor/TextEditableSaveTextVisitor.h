#pragma once

#include "origo/assets/Script.h"
#include "origo/assets/Shader.h"

#include "origo/scripting/ScriptSystem.h"

#include "systems/text_editor/TextEditablePathVisitor.h"

namespace OrigoEditor {

struct TextEditableSaveTextVisitor {
	std::string Text;

	void operator()(Origo::Script* script) const {
		std::ofstream out(script->GetPath());
		out << Text;
		Origo::ScriptSystem::Register(script->GetID(), script->GetPath(), Text);
		Origo::ScriptSystem::ReloadAllNecessary();
	}

	void operator()(Origo::Shader* shader) const {
		auto pathOpt = TextEditablePathVisitor {}(shader);

		if (!pathOpt) {
			ORG_ERROR("Shader has no editable path");
			return;
		}

		{
			std::ofstream out(*pathOpt, std::ios::binary);
			if (!out.is_open()) {
				ORG_ERROR("Failed to open shader file: {}", pathOpt->string());
				return;
			}

			out << Text;
			out.flush();
		}

		shader->UnuseProgram();
		shader->Resolve();
	}
};
}
