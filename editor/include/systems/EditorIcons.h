#pragma once

#include <string>
#include <unordered_map>

#include <imgui.h>

#include "origo/assets/Texture2D.h"

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
	Step
};

class EditorIcons {
public:
	static void Init();
	static void Shutdown();

	static ImTextureID Get(Origo::AssetType type);
	static ImTextureID Get(IconType type);
	static Origo::Ref<Origo::Texture2D> GetTexture(IconType type);

private:
	static Origo::Ref<Origo::Texture2D> LoadSVG(
	    const std::string& path,
	    int size = 16);

private:
	static std::unordered_map<IconType, Origo::Ref<Origo::Texture2D>> s_Icons;
	static bool s_Initialized;
};
}
