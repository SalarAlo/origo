#pragma once

#include "origo/core/Layer.h"

#include "origo/imgui/ImGuiLayer.h"

#include "panels/PanelManager.h"

#include "state/EditorContext.h"

namespace OrigoEditor {

class EditorUILayer : public Origo::Layer {
public:
	EditorUILayer(EditorContext& ctx, Origo::ImGuiLayer& controller);

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(double dt) override;
	void OnEvent(Origo::Event& e) override;

private:
	Origo::ImGuiLayer& m_ImGuiController;
	EditorContext& m_Context;
	PanelManager m_PanelManager;

	int m_HierarchyId {};
	int m_InspectorId {};
};

}
