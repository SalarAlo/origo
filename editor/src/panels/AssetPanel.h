#pragma once

#include <string>
#include <vector>

#include "EditorPanel.h"

#include "panels/EditorAssetTree.h"

#include "state/EditorContext.h"

namespace OrigoEditor {

class AssetPanel : public EditorPanel {
public:
	AssetPanel(EditorContext& ctx);
	const char* GetName() const override { return "Assets"; }
	void OnImGuiRender() override;

private:
	void DrawTopBar();
	void DrawBreadcrumbBar(float availableWidth);
	void DrawFolderContents(FolderEntry* folder);
	void DrawFolderTile(FolderEntry* folder, ImDrawList* drawList);
	void DrawAssetTile(AssetEntry*& asset, ImDrawList* drawList);

private:
	EditorContext& m_Context;
	EditorAssetTree m_Tree;

	FolderEntry* m_CurrentFolder = nullptr;
	std::vector<FolderEntry*> m_Path;

	std::string m_Search;
};

}
