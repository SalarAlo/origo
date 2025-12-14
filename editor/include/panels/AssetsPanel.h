#pragma once

#include "EditorPanel.h"
#include "imgui.h"

namespace OrigoEditor {

struct Icon {
	GLuint TextureID = 0;
	int Width = 0;
	int Height = 0;
};

struct AssetsPanelState {
	std::filesystem::path ProjectRoot { "./" };
	bool Initialized = false;

	Icon DirectoryIcon;
	Icon FileIcon;
	Icon ImageIcon;
	Icon ScriptIcon;
};

class AssetsPanel : public EditorPanel {
public:
	~AssetsPanel() override;
	const char* GetName() const override { return "Assets"; }

	void OnImGuiRender() override;

private:
	void InitAssetsPanel();
	bool IsInsideProject(const std::filesystem::path& p);
	ImTextureID GetIconForFile(const std::filesystem::path& p);
	void DrawFileSystemRecursive(const std::filesystem::path& path);

private:
	AssetsPanelState m_State {};
};

}
