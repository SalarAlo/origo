#pragma once

#include "EditorPanel.h"
#include "origo/assets/Texture.h"

namespace OrigoEditor {

struct AssetsPanelState {
	std::filesystem::path ProjectRoot { "./" };
	bool Initialized = false;

	Origo::Ref<Origo::Texture> DirectoryIcon;
	Origo::Ref<Origo::Texture> FileIcon;
	Origo::Ref<Origo::Texture> ImageIcon;
	Origo::Ref<Origo::Texture> ScriptIcon;
};

class AssetsPanel : public EditorPanel {
public:
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
