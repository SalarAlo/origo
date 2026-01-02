#include "state/EditorContext.h"
#include "systems/EditorSelectionSystem.h"

namespace OrigoEditor {

void EditorContext::SetSelectedEntity(const Origo::RID& entity) {
	m_SelectedEntity = entity;
	EditorSelectionSystem::Apply(*this);
}

void EditorContext::UnselectSelectedEntity() {
	m_SelectedEntity = std::nullopt;
	EditorSelectionSystem::Apply(*this);
}

};
