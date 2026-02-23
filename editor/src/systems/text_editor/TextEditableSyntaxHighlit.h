
#pragma once

#include "TextEditor.h"

#include "origo/assets/Script.h"
#include "origo/assets/Shader.h"

namespace OrigoEditor {

struct TextEditableSyntaxHighlit {
	const TextEditor::LanguageDefinition& operator()(Origo::Script* script) const {
		return TextEditor::LanguageDefinition::Lua();
	}

	const TextEditor::LanguageDefinition& operator()(Origo::Shader* shader) const {
		return TextEditor::LanguageDefinition::GLSL();
	}
};
}
