#include "state/EditorContext.h"

#include "systems/EditorSelectionSystem.h"

namespace OrigoEditor {

void EditorContext::set_selected_entity(const Origo::RID& entity) {
	m_selected_entity = entity;
	EditorSelectionSystem::apply(*this);
}

void EditorContext::unselect_entity() {
	m_selected_entity = std::nullopt;
	EditorSelectionSystem::apply(*this);
}

void EditorContext::unselect_asset() {
	m_selected_asset = std::nullopt;
}

void EditorContext::set_selected_asset(const Origo::UUID& uuid) {
	m_selected_asset = uuid;
}

};
