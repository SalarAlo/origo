#pragma once

#include "EditorPanel.h"
#include "origo/scene/Scene.h"

namespace OrigoEditor {

class HierarchyPanel : public EditorPanel {
public:
	HierarchyPanel(Origo::Scene& scene)
	    : m_Scene(scene) { }

	const char* GetName() const override { return "Hierarchy"; }
	Origo::Entity* GetSelectedEntity() { return m_Selected; }

	void OnImGuiRender() override;

private:
	Origo::Scene& m_Scene;
	Origo::Entity* m_Selected = nullptr;
};

}
