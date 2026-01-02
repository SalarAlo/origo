#include "systems/EditorSelectionSystem.h"
#include "components/EditorOutline.h"

namespace OrigoEditor::EditorSelectionSystem {

void Apply(EditorContext& ctx) {
	for (auto entity : ctx.ActiveScene->GetEntities()) {
		auto outline = ctx.ActiveScene->GetNativeComponent<EditorOutline>(entity);

		if (!outline)
			continue;

		outline->ShouldOutline = ctx.GetSelectedEntity() && ctx.GetSelectedEntity() == entity;
	}
}

}
