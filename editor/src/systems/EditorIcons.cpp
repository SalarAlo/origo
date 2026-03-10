#include "systems/EditorIcons.h"

#include "origo/assets/Asset.h"
#include "origo/assets/TextureSource.h"
#include "origo/core/PathContext.h"

namespace OrigoEditor {

static ImTextureID to_im_texture_id(const Origo::Ref<Origo::Texture2D>& tex) {
	return (ImTextureID)(intptr_t)tex->get_renderer_id();
}

void EditorIcons::init() {
	if (m_initialized)
		return;

	m_icons[IconType::Entity] = load_svg("icons/Entity.svg");
	m_icons[IconType::Unknown] = m_icons[IconType::Entity];
	m_icons[IconType::Script] = load_svg("icons/Script.svg");

	m_icons[IconType::Folder] = load_svg("icons/Folder.svg");
	m_icons[IconType::File] = load_svg("icons/File.svg");
	m_icons[IconType::Image] = load_svg("icons/Image.svg");

	m_icons[IconType::Play] = load_svg("icons/Play.svg", 18);
	m_icons[IconType::Pause] = load_svg("icons/Pause.svg", 18);
	m_icons[IconType::Stop] = load_svg("icons/Stop.svg", 18);
	m_icons[IconType::Step] = load_svg("icons/Step.svg", 18);
	m_icons[IconType::Resume] = load_svg("icons/Resume.svg", 18);

	m_icons[IconType::Texture2D] = m_icons[IconType::Image];
	m_icons[IconType::Mesh] = load_svg("icons/Mesh.svg", 18);
	m_icons[IconType::Shader] = load_svg("icons/Shader.svg", 18);

	m_icons[IconType::Menu] = load_svg("icons/Menu.svg", 18);

	m_initialized = true;
}

ImTextureID EditorIcons::get(Origo::AssetType type) {
	switch (type) {
	case Origo::AssetType::Shader:
		return get(IconType::Shader);

	case Origo::AssetType::Texture2D:
	case Origo::AssetType::TextureCubemap:
		return get(IconType::Image);

	case Origo::AssetType::MaterialPBR:
	case Origo::AssetType::SkyboxMaterial:
		return get(IconType::Unknown);

	case Origo::AssetType::Mesh:
		return get(IconType::Mesh);

	case Origo::AssetType::Model:
		return get(IconType::Entity);

	case Origo::AssetType::Script:
		return get(IconType::Script);

	default:
		return get(IconType::Unknown);
	}
}

void EditorIcons::shutdown() {
	m_icons.clear();
	m_initialized = false;
}

ImTextureID EditorIcons::get(IconType type) {
	if (!m_initialized)
		init();

	auto it = m_icons.find(type);
	if (it == m_icons.end()) {
		ORG_CORE_WARN("EditorIcons::Get: Asset Icon of type {} not found", magic_enum::enum_name(type));
		return 0;
	}

	return to_im_texture_id(it->second);
}

Origo::Ref<Origo::Texture2D> EditorIcons::get_texture(IconType type) {
	auto it = m_icons.find(type);
	if (it == m_icons.end())
		return nullptr;

	return it->second;
}

Origo::Ref<Origo::Texture2D>
EditorIcons::load_svg(const std::string& path, int size) {
	auto tex = Origo::make_ref<Origo::Texture2D>(Origo::TextureType::UI);
	tex->set_source(
	    Origo::make_scope<Origo::TextureSourceSVG>(
	        Origo::PathContextService::get_instance().editor().resolve(path).string(),
	        size,
	        size));
	tex->load();
	return tex;
}

}
