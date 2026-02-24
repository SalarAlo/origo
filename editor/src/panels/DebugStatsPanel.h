#pragma once

#include "panels/EditorPanel.h"

#include "state/EditorContext.h"

namespace OrigoEditor {
class DebugStatsPanel final : public EditorPanel {
public:
	DebugStatsPanel(EditorContext& ctx)
	    : m_Context(ctx) { }

	bool IsCollapsable() const override { return true; }

	const char* GetName() const override { return "Debug Stats"; }
	void OnImGuiRender() override;

private:
	EditorContext& m_Context;
};

}
