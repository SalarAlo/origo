#include "state/EditorContext.h"
#include "systems/EditorSelectionSystem.h"

namespace OrigoEditor {

void EditorContext::SetSelectedEntity(const Origo::RID& entity) {
	m_SelectedEntity = entity;
	EditorSelectionSystem::Apply(*this);
}

void EditorContext::UnselectEntity() {
	m_SelectedEntity = std::nullopt;
	EditorSelectionSystem::Apply(*this);
}

void EditorContext::UnselectAsset() {
	m_SelectedAsset = std::nullopt;
}

void EditorContext::SetSelectedAsset(const Origo::UUID& uuid) {
	m_SelectedAsset = uuid;
}

};
