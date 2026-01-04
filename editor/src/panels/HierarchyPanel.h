#pragma once

#include "state/EditorContext.h"
#include "EditorPanel.h"

namespace OrigoEditor {

class HierarchyPanel : public EditorPanel {
public:
	HierarchyPanel(EditorContext& ctx);

	const char* GetName() const override { return "Hierarchy"; }

	void OnImGuiRender() override;

private:
	void ChangeActiveSelectedEntity(const Origo::RID& e);

private:
	EditorContext& m_Context;
};

}
