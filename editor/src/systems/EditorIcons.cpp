#include "systems/EditorIcons.h"

#include "origo/assets/Asset.h"
#include "origo/assets/TextureSource.h"

namespace OrigoEditor {

static ImTextureID ToImTextureID(const Origo::Ref<Origo::Texture2D>& tex) {
	return (ImTextureID)(intptr_t)tex->GetRendererID();
}

void EditorIcons::Init() {
	if (m_Initialized)
		return;

	m_Icons[IconType::Entity] = LoadSVG("icons/Entity.svg");
	m_Icons[IconType::Unknown] = m_Icons[IconType::Entity];
	m_Icons[IconType::Script] = LoadSVG("icons/Script.svg");

	m_Icons[IconType::Folder] = LoadSVG("icons/Folder.svg");
	m_Icons[IconType::File] = LoadSVG("icons/File.svg");
	m_Icons[IconType::Image] = LoadSVG("icons/Image.svg");

	m_Icons[IconType::Play] = LoadSVG("icons/Play.svg", 18);
	m_Icons[IconType::Pause] = LoadSVG("icons/Pause.svg", 18);
	m_Icons[IconType::Stop] = LoadSVG("icons/Stop.svg", 18);
	m_Icons[IconType::Step] = LoadSVG("icons/Step.svg", 18);
	m_Icons[IconType::Resume] = LoadSVG("icons/Resume.svg", 18);

	m_Icons[IconType::Texture2D] = m_Icons[IconType::Image];
	m_Icons[IconType::Mesh] = LoadSVG("icons/Mesh.svg", 18);
	m_Icons[IconType::Shader] = LoadSVG("icons/Shader.svg", 18);

	m_Icons[IconType::Menu] = LoadSVG("icons/Menu.svg", 18);

	m_Initialized = true;
}

ImTextureID EditorIcons::Get(Origo::AssetType type) {
	switch (type) {
	case Origo::AssetType::Shader:
		return Get(IconType::Shader);

	case Origo::AssetType::Texture2D:
	case Origo::AssetType::TextureCubemap:
		return Get(IconType::Image);

	case Origo::AssetType::Material2D:
	case Origo::AssetType::SkyboxMaterial:
		return Get(IconType::Unknown);

	case Origo::AssetType::Mesh:
		return Get(IconType::Mesh);

	case Origo::AssetType::Model:
		return Get(IconType::Entity);

	case Origo::AssetType::Script:
		return Get(IconType::Script);

	default:
		return Get(IconType::Unknown);
	}
}

void EditorIcons::Shutdown() {
	m_Icons.clear();
	m_Initialized = false;
}

ImTextureID EditorIcons::Get(IconType type) {
	if (!m_Initialized)
		Init();

	auto it = m_Icons.find(type);
	if (it == m_Icons.end()) {
		ORG_CORE_WARN("EditorIcons::Get: Asset Icon of type {} not found", magic_enum::enum_name(type));
		return 0;
	}

	return ToImTextureID(it->second);
}

Origo::Ref<Origo::Texture2D> EditorIcons::GetTexture(IconType type) {
	auto it = m_Icons.find(type);
	if (it == m_Icons.end())
		return nullptr;

	return it->second;
}

Origo::Ref<Origo::Texture2D>
EditorIcons::LoadSVG(const std::string& path, int size) {
	auto tex = Origo::MakeRef<Origo::Texture2D>(Origo::TextureType::UI);
	tex->SetSource(
	    Origo::MakeScope<Origo::TextureSourceSVG>(path, size, size));
	tex->Load();
	return tex;
}

}
