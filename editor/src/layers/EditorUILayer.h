#pragma once

#include "origo/core/Layer.h"

#include "origo/imgui/ImGuiLayer.h"

#include "panels/PanelManager.h"

#include "state/EditorContext.h"

namespace OrigoEditor {

class EditorUILayer : public Origo::Layer {
public:
	EditorUILayer(EditorContext& ctx, Origo::ImGuiLayer& controller);

	void on_attach() override;
	void on_detach() override;
	void on_update(double dt) override;
	void on_event(Origo::Event& e) override;

private:
	Origo::ImGuiLayer& m_im_gui_controller;
	EditorContext& m_context;
	PanelManager m_panel_manager;

	int m_hierarchy_id {};
	int m_inspector_id {};
};

}
