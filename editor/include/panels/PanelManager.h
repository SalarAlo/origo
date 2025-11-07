#pragma once

#include "panels/EditorPanel.h"
#include <concepts>
#include <vector>

namespace OrigoEditor {

template <typename T>
concept EditorPanelDerived = std::derived_from<T, EditorPanel>;

class PanelManager {
public:
	template <EditorPanelDerived T, typename... Args>
	int AddPanel(Args&&... args) {
		m_Panels.push_back(
		    std::unique_ptr<EditorPanel>(new T(std::forward<Args>(args)...)));
		return static_cast<int>(m_Panels.size()) - 1;
	}

	EditorPanel* GetPanel(int id) {
		if (id < 0 || id >= static_cast<int>(m_Panels.size())) {
			ORG_ERROR("Invalid panel ID: {} (count = {})", id, m_Panels.size());
			return nullptr;
		}
		return m_Panels[id].get();
	}

	void RenderPanels();
	void RenderMenuItems();

private:
	std::vector<Origo::Scope<EditorPanel>> m_Panels;
};

}
