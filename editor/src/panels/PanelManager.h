#pragma once

#include <concepts>

#include "panels/EditorPanel.h"

#include "state/EditorContext.h"

namespace OrigoEditor {

template <typename T>
concept EditorPanelDerived = std::derived_from<T, EditorPanel>;

class PanelManager {
public:
	PanelManager(EditorContext& ctx);

	template <EditorPanelDerived T, typename... Args>
	int add_panel(Args&&... args) {
		m_panels.push_back(
		    std::unique_ptr<EditorPanel>(new T(std::forward<Args>(args)...)));
		return static_cast<int>(m_panels.size()) - 1;
	}

	EditorPanel* get_panel(int id) {
		if (id < 0 || id >= static_cast<int>(m_panels.size())) {
			ORG_ERROR("Invalid panel ID: {} (count = {})", id, m_panels.size());
			return nullptr;
		}
		return m_panels[id].get();
	}

	void render_panels();
	void render_menu_items();

private:
	EditorContext& m_context;
	std::vector<Origo::Scope<EditorPanel>> m_panels;
};

}
