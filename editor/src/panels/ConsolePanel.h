
#pragma once

#include "EditorPanel.h"
#include <spdlog/spdlog.h>

namespace OrigoEditor {

class ConsolePanel : public EditorPanel {
public:
	const char* GetName() const override { return "Console"; }
	void OnImGuiRender() override;

private:
	void DrawToolbar();
	void DrawLogView();

	static ImVec4 LevelToColor(spdlog::level::level_enum level);
	static const char* LevelToString(spdlog::level::level_enum level);

private:
	bool m_AutoScroll {};
};

}
