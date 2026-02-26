#pragma once

#include <imgui.h>

#include <string>
#include <unordered_map>

#include "origo/assets/Texture2D.h"

#include "origo/utils/Singleton.h"

namespace OrigoEditor {

enum class IconType {
	Unknown,
	Entity,
	Script,
	Folder,
	File,
	Image,
	Texture2D,
	Mesh,
	Material,
	Shader,
	Scene,
	Play,
	Pause,
	Resume,
	Stop,
	Step,
	Menu
};

class EditorIcons : public Origo::Singleton<EditorIcons> {
public:
	void init();
	void shutdown();

	ImTextureID get(Origo::AssetType type);
	ImTextureID get(IconType type);
	Origo::Ref<Origo::Texture2D> get_texture(IconType type);

private:
	Origo::Ref<Origo::Texture2D> load_svg(const std::string& path, int size = 16);

private:
	std::unordered_map<IconType, Origo::Ref<Origo::Texture2D>> m_icons {};
	bool m_initialized {};
};
}
