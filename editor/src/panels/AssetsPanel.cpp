#include "panels/AssetsPanel.h"

#include "imgui.h"
#include <filesystem>
#include <string>

#include <glad/glad.h>

#include "nanosvg.h"
#include "nanosvgrast.h"

namespace OrigoEditor {

struct Icon {
	GLuint TextureID = 0;
	int Width = 0;
	int Height = 0;
};

static std::filesystem::path s_ProjectRoot;
static bool s_Initialized = false;

static Icon s_IconFolder;
static Icon s_IconFile;
static Icon s_IconImage;
static Icon s_IconScript;

static bool IsInsideProject(const std::filesystem::path& p) {
	auto abs = std::filesystem::weakly_canonical(p);
	auto root = std::filesystem::weakly_canonical(s_ProjectRoot);

	auto absStr = abs.generic_string();
	auto rootStr = root.generic_string();

	if (absStr.size() < rootStr.size())
		return false;

	return absStr.compare(0, rootStr.size(), rootStr) == 0;
}

static GLuint CreateGLTextureRGBA(const unsigned char* pixels, int width, int height) {
	GLuint texId = 0;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
	return texId;
}

static Icon LoadSVG(const std::string& path, int size = 18) {
	Icon icon {};

	NSVGimage* svg = nsvgParseFromFile(path.c_str(), "px", 96.0f);
	if (!svg)
		return icon;

	NSVGrasterizer* rast = nsvgCreateRasterizer();
	if (!rast) {
		nsvgDelete(svg);
		return icon;
	}

	int w = size;
	int h = size;

	float baseWidth = (svg->width > 0.0f ? svg->width : (float)size);
	float scale = (float)size / baseWidth;

	unsigned char* pixels = (unsigned char*)malloc(w * h * 4);
	if (!pixels) {
		nsvgDeleteRasterizer(rast);
		nsvgDelete(svg);
		return icon;
	}

	nsvgRasterize(rast, svg, 0.0f, 0.0f, scale, pixels, w, h, w * 4);

	icon.TextureID = CreateGLTextureRGBA(pixels, w, h);
	icon.Width = w;
	icon.Height = h;

	free(pixels);
	nsvgDeleteRasterizer(rast);
	nsvgDelete(svg);

	return icon;
}

static inline ImTextureID ToImTextureID(GLuint tex) {
	return (ImTextureID)(intptr_t)tex;
}

static void DrawIconAligned(const char* label, ImTextureID icon) {
	float iconSize = 16.0f;

	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 textSize = ImGui::CalcTextSize(label);

	float iconY = pos.y;

	ImGui::SetCursorScreenPos(ImVec2(pos.x, iconY));
	ImGui::Image(icon, ImVec2(iconSize, iconSize));

	ImGui::SetCursorScreenPos(ImVec2(pos.x + iconSize + 4, pos.y));
}

static bool TreeNodeWithIcon(const char* label, ImTextureID icon, ImGuiTreeNodeFlags flags) {
	ImGui::PushID(label);

	ImGuiStyle& style = ImGui::GetStyle();
	float oldPadY = style.FramePadding.y;
	style.FramePadding.y = 1.0f;

	bool open = ImGui::TreeNodeEx("##node", flags);

	style.FramePadding.y = oldPadY;

	ImGui::SameLine();
	DrawIconAligned(label, icon);
	ImGui::SameLine();
	ImGui::TextUnformatted(label);

	ImGui::PopID();
	return open;
}

static void TreeLeafWithIcon(const char* label, ImTextureID icon) {
	ImGui::PushID(label);

	ImGui::TreeNodeEx("##leaf",
	    ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);

	ImGui::SameLine();
	DrawIconAligned(label, icon);

	ImGui::SameLine();
	ImGui::TextUnformatted(label);

	ImGui::PopID();
}

static ImTextureID GetIconForFile(const std::filesystem::path& p) {
	auto ext = p.extension().string();

	if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga")
		return ToImTextureID(s_IconImage.TextureID);

	if (ext == ".cs" || ext == ".cpp" || ext == ".c" || ext == ".h" || ext == ".hpp" || ext == ".gd")
		return ToImTextureID(s_IconScript.TextureID);

	return ToImTextureID(s_IconFile.TextureID);
}

static void DrawFileSystemRecursive(const std::filesystem::path& path) {
	if (!IsInsideProject(path))
		return;

	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		if (!entry.is_directory())
			continue;

		auto p = entry.path();
		auto name = p.filename().string();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding;

		bool open = TreeNodeWithIcon(name.c_str(), ToImTextureID(s_IconFolder.TextureID), flags);

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
			std::string s = p.string();
			ImGui::SetDragDropPayload("ASSET_PATH", s.c_str(), s.size() + 1);
			ImGui::Text("%s", name.c_str());
			ImGui::EndDragDropSource();
		}

		if (open) {
			DrawFileSystemRecursive(p);
			ImGui::TreePop();
		}
	}

	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		if (!entry.is_regular_file())
			continue;

		auto p = entry.path();
		auto name = p.filename().string();

		if (p.extension() == ".asset" || p.extension() == ".meta")
			continue;

		ImTextureID icon = GetIconForFile(p);
		TreeLeafWithIcon(name.c_str(), icon);

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
			std::string s = p.string();
			ImGui::SetDragDropPayload("ASSET_PATH", s.c_str(), s.size() + 1);
			ImGui::Text("%s", name.c_str());
			ImGui::EndDragDropSource();
		}
	}
}

void AssetsPanel::OnImGuiRender() {
	if (!s_Initialized) {
		s_ProjectRoot = std::filesystem::current_path();

		s_IconFolder = LoadSVG("./icons/Folder.svg", 18);
		s_IconFile = LoadSVG("./icons/File.svg", 18);
		s_IconImage = LoadSVG("./icons/Image.svg", 18);
		s_IconScript = LoadSVG("./icons/Script.svg", 18);

		s_Initialized = true;
	}

	ImGui::BeginChild("FileSystemPanel", ImVec2(0, 0), true);

	ImGuiTreeNodeFlags rootFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding;

	bool open = TreeNodeWithIcon(
	    s_ProjectRoot.filename().string().c_str(),
	    ToImTextureID(s_IconFolder.TextureID),
	    rootFlags);

	if (open) {
		DrawFileSystemRecursive(s_ProjectRoot);
		ImGui::TreePop();
	}

	ImGui::EndChild();

	if (ImGui::BeginPopupContextWindow("AssetsPanelContext", ImGuiPopupFlags_MouseButtonRight)) {
		if (ImGui::MenuItem("Create Folder")) { }
		if (ImGui::MenuItem("Import Asset")) { }
		if (ImGui::MenuItem("Delete")) { }
		ImGui::EndPopup();
	}
}

}
