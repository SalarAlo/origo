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
	void Init();
	void Shutdown();

	ImTextureID Get(Origo::AssetType type);
	ImTextureID Get(IconType type);
	Origo::Ref<Origo::Texture2D> GetTexture(IconType type);

private:
	Origo::Ref<Origo::Texture2D> LoadSVG(const std::string& path, int size = 16);

private:
	std::unordered_map<IconType, Origo::Ref<Origo::Texture2D>> m_Icons {};
	bool m_Initialized {};
};
}
