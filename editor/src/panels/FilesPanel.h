#pragma once

#include "EditorPanel.h"

#include <filesystem>
#include <imgui.h>

namespace OrigoEditor {

struct FilesPanelState {
	std::filesystem::path ProjectRoot { "./" };
};

class FilesPanel : public EditorPanel {
public:
	const char* get_name() const override { return "Files"; }
	void on_im_gui_render() override;

private:
	bool is_inside_project(const std::filesystem::path& p);
	ImTextureID get_icon_for_file(const std::filesystem::path& p);
	void draw_file_system_recursive(const std::filesystem::path& path);

private:
	FilesPanelState m_state {};
};

}
