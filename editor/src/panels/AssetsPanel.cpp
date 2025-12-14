#include "panels/AssetsPanel.h"

#include "imgui.h"
#include <filesystem>
#include <string>
#include <vector>

#include <glad/glad.h>

#include "nanosvg.h"
#include "nanosvgrast.h"

namespace OrigoEditor {

static void DestroyIcon(Icon& icon) {
	if (icon.TextureID) {
		glDeleteTextures(1, &icon.TextureID);
		icon.TextureID = 0;
	}
	icon.Width = icon.Height = 0;
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

	std::vector<unsigned char> pixels(w * h * 4);
	nsvgRasterize(rast, svg, 0.0f, 0.0f, scale, pixels.data(), w, h, w * 4);

	icon.TextureID = CreateGLTextureRGBA(pixels.data(), w, h);
	icon.Width = w;
	icon.Height = h;

	nsvgDeleteRasterizer(rast);
	nsvgDelete(svg);

	return icon;
}

static inline ImTextureID ToImTextureID(GLuint tex) {
	return (ImTextureID)(intptr_t)tex;
}

static bool TreeNodeWithIcon(const char* label, ImTextureID icon, ImGuiTreeNodeFlags flags) {
	ImGui::PushID(label);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, 1.0f));

	bool open = ImGui::TreeNodeEx("##node", flags);

	ImGui::PopStyleVar();

	ImGui::SameLine();
	ImGui::Image(icon, ImVec2(16, 16));
	ImGui::SameLine(0.0f, 6.0f);
	ImGui::TextUnformatted(label);

	ImGui::PopID();
	return open;
}

static void TreeLeafWithIcon(const char* label, ImTextureID icon) {
	ImGui::PushID(label);

	ImGui::TreeNodeEx(
	    "##leaf",
	    ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);

	ImGui::SameLine();
	ImGui::Image(icon, ImVec2(16, 16));
	ImGui::SameLine(0.0f, 6.0f);
	ImGui::TextUnformatted(label);

	ImGui::PopID();
}

ImTextureID AssetsPanel::GetIconForFile(const std::filesystem::path& p) {
	const auto ext = p.extension();

	if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga")
		return ToImTextureID(m_State.ImageIcon.TextureID);

	if (ext == ".cs" || ext == ".cpp" || ext == ".c" || ext == ".h" || ext == ".hpp" || ext == ".gd" || ext == ".glsl")
		return ToImTextureID(m_State.ScriptIcon.TextureID);

	return ToImTextureID(m_State.FileIcon.TextureID);
}

bool AssetsPanel::IsInsideProject(const std::filesystem::path& p) {
	auto abs = std::filesystem::weakly_canonical(p);
	auto root = std::filesystem::weakly_canonical(m_State.ProjectRoot);

	auto absStr = abs.generic_string();
	auto rootStr = root.generic_string();

	if (absStr.size() < rootStr.size())
		return false;

	return absStr == rootStr || (absStr.size() > rootStr.size() && absStr.compare(0, rootStr.size(), rootStr) == 0 && absStr[rootStr.size()] == '/');
}

void AssetsPanel::DrawFileSystemRecursive(const std::filesystem::path& path) {
	if (!IsInsideProject(path))
		return;

	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		if (entry.is_directory()) {

			const auto p = entry.path();
			const auto name = p.filename().string();

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding;

			bool open = TreeNodeWithIcon(
			    name.c_str(),
			    ToImTextureID(m_State.DirectoryIcon.TextureID),
			    flags);

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
				std::string s = p.string();
				ImGui::SetDragDropPayload("ASSET_PATH", s.c_str(), s.size() + 1);
				ImGui::TextUnformatted(name.c_str());
				ImGui::EndDragDropSource();
			}

			if (open) {
				DrawFileSystemRecursive(p);
				ImGui::TreePop();
			}
		} else if (entry.is_regular_file()) {
			const auto p = entry.path();
			const auto name = p.filename().string();

			if (p.extension() == ".asset" || p.extension() == ".meta")
				continue;

			ImTextureID icon = GetIconForFile(p);
			TreeLeafWithIcon(name.c_str(), icon);

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
				std::string s = p.string();
				ImGui::SetDragDropPayload("ASSET_PATH", s.c_str(), s.size() + 1);
				ImGui::TextUnformatted(name.c_str());
				ImGui::EndDragDropSource();
			}
		}
	}
}

void AssetsPanel::OnImGuiRender() {
	if (!m_State.Initialized)
		InitAssetsPanel();

	ImGui::BeginChild("FileSystemPanel", ImVec2(0, 0), true);

	ImGuiTreeNodeFlags rootFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding;

	bool open = TreeNodeWithIcon(
	    m_State.ProjectRoot.filename().string().c_str(),
	    ToImTextureID(m_State.DirectoryIcon.TextureID),
	    rootFlags);

	if (open) {
		DrawFileSystemRecursive(m_State.ProjectRoot);
		ImGui::TreePop();
	}

	if (ImGui::BeginPopupContextWindow("AssetsPanelContext", ImGuiPopupFlags_MouseButtonRight)) {
		ImGui::MenuItem("Create Folder");
		ImGui::MenuItem("Import Asset");
		ImGui::MenuItem("Delete");
		ImGui::EndPopup();
	}

	ImGui::EndChild();
}

void AssetsPanel::InitAssetsPanel() {
	m_State.ProjectRoot = std::filesystem::current_path();

	m_State.DirectoryIcon = LoadSVG("icons/Folder.svg");
	m_State.FileIcon = LoadSVG("icons/File.svg");
	m_State.ImageIcon = LoadSVG("icons/Image.svg");
	m_State.ScriptIcon = LoadSVG("icons/Script.svg");

	m_State.Initialized = true;
}

AssetsPanel::~AssetsPanel() {
	DestroyIcon(m_State.DirectoryIcon);
	DestroyIcon(m_State.FileIcon);
	DestroyIcon(m_State.ImageIcon);
	DestroyIcon(m_State.ScriptIcon);
}

}
