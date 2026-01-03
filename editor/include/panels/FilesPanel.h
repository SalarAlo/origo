#pragma
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
	const char* GetName() const override { return "Files"; }
	void OnImGuiRender() override;

private:
	bool IsInsideProject(const std::filesystem::path& p);
	ImTextureID GetIconForFile(const std::filesystem::path& p);
	void DrawFileSystemRecursive(const std::filesystem::path& path);

private:
	FilesPanelState m_State {};
};

}
