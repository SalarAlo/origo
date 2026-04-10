#include "systems/EditorSelectionSystem.h"

#include "components/EditorOutlineComponent.h"

namespace OrigoEditor::EditorSelectionSystem {

void apply(EditorContext& ctx) {
	for (auto entity : ctx.ActiveScene->get_entities()) {
		auto outline = ctx.ActiveScene->get_native_component<EditorOutlineComponent>(entity);

		if (!outline)
			continue;

		outline->ShouldOutline = ctx.get_selected_entity() && ctx.get_selected_entity() == entity;
	}
}

}
