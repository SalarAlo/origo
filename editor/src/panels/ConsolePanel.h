
#pragma once

#include "EditorPanel.h"
#include <spdlog/spdlog.h>

namespace OrigoEditor {

class ConsolePanel : public EditorPanel {
public:
	const char* get_name() const override { return "Console"; }
	void on_im_gui_render() override;

private:
	void draw_toolbar();
	void draw_log_view();

	static ImVec4 level_to_color(spdlog::level::level_enum level);
	static const char* level_to_string(spdlog::level::level_enum level);
	static bool should_display_level(
	    spdlog::level::level_enum level,
	    spdlog::level::level_enum minimum_level);

private:
	bool m_auto_scroll {};
	spdlog::level::level_enum m_warn_level { spdlog::level::warn };
};

}
