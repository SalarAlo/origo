#pragma once

#include "EditorContext.h"
#include "origo/core/Layer.h"
#include "origo/imgui/ImGuiLayer.h"
#include "panels/PanelManager.h"

namespace OrigoEditor {

class EditorUILayer : public Origo::Layer {
public:
	EditorUILayer(EditorContext& ctx);

	void OnAttach() override;
	void OnUpdate(double dt) override;
	void OnEvent(Origo::Event& e) override;

private:
	Origo::ImGuiLayer m_ImGuiLayer {};
	EditorContext m_Context;
	PanelManager m_PanelManager;

	int m_HierarchyId {};
	int m_InspectorId {};
};

}
