#pragma once

#include "EditorPanel.h"
#include "origo/scene/Entity.hpp"
#include "origo/scene/Scene.h"

namespace OrigoEditor {

class InspectorPanel : public EditorPanel {
public:
	InspectorPanel(Origo::Scene& scene)
	    : m_Scene(scene) { }

	const char* GetName() const override { return "Inspector"; }

	void SetSelectedEntity(Origo::Entity* e) { m_SelectedEntity = e; }
	void OnImGuiRender() override;

private:
	Origo::Scene& m_Scene;
	Origo::Entity* m_SelectedEntity;
};

}
