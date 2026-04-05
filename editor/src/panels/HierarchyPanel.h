#pragma once

#include <string>

#include "EditorPanel.h"

#include "state/EditorContext.h"

namespace OrigoEditor {

class HierarchyPanel : public EditorPanel {
public:
	HierarchyPanel(EditorContext& ctx);

	const char* get_name() const override { return "Hierarchy"; }

	void on_im_gui_render() override;
	void handle_prefab_drop_target();

private:
	void change_active_selected_entity(const Origo::RID& e);

private:
	EditorContext& m_context;
	std::string m_search_query {};
};

}
