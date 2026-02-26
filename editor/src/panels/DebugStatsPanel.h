#pragma once

#include "panels/EditorPanel.h"

#include "state/EditorContext.h"

#include <array>

namespace OrigoEditor {
class DebugStatsPanel final : public EditorPanel {
public:
	DebugStatsPanel(EditorContext& ctx)
	    : m_context(ctx) { }

	bool is_collapsable() const override { return true; }

	const char* get_name() const override { return "Debug Stats"; }
	void on_im_gui_render() override;

private:
	EditorContext& m_context;
	static constexpr size_t k_frame_history = 180;
	std::array<float, k_frame_history> m_frame_times_ms {};
	size_t m_frame_index = 0;
	size_t m_frame_count = 0;
};

}
