#pragma once

#include "EditorContext.h"
#include "EditorPanel.h"

namespace OrigoEditor {

class HierarchyPanel : public EditorPanel {
public:
	HierarchyPanel(EditorContext& ctx)
	    : m_Context(ctx) { }

	const char* GetName() const override { return "Hierarchy"; }

	void OnImGuiRender() override;

private:
	void ChangeActiveSelectedEntity(Origo::Entity& e);

private:
	EditorContext& m_Context;
};

}
