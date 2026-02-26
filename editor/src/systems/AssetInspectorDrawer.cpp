#include "systems/AssetInspectorDrawer.h"

#include "imgui.h"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Material2D.h"

#include "ui/ComponentUI.h"

namespace OrigoEditor {

void AssetInspectorDrawer::draw_asset(const Origo::AssetMetadata& md) {
	auto& am { Origo::AssetManager::get_instance() };
	auto asset_handle { am.get_handle_by_uuid(*md.ID) };
	if (!asset_handle) {
		ORG_CORE_TRACE("Trying to draw invalid asset");
		return;
	}

	auto asset_ptr { am.get(*asset_handle) };
	ImGui::Text("%s", (*md.ID).to_string().c_str());
	draw_specific(asset_ptr, md.Type);
}

void AssetInspectorDrawer::draw_specific(Origo::Asset* asset, Origo::AssetType type) {
	switch (type) {
	case Origo::AssetType::Material2D:
		draw_material(dynamic_cast<Origo::Material2D*>(asset));
		break;
	default: {
		std::string asset_type_str { magic_enum::enum_name(type) };
		ImGui::Text("%s", ("No draw for assets of type " + asset_type_str + "implemented yet").c_str());
	}
	}
}

void AssetInspectorDrawer::draw_material(Origo::Material2D* material) {
	auto albedo_handle { material->get_albedo() };
	auto shader_handle { material->get_shader() };

	ComponentUI::draw_asset_control("Albedo", albedo_handle, Origo::AssetType::Texture2D);
	ComponentUI::draw_asset_control("Shader", shader_handle, Origo::AssetType::Shader);

	if (albedo_handle.has_value())
		material->set_albedo(*albedo_handle);

	if (shader_handle.has_value())
		material->set_shader(*shader_handle);

	Origo::Vec3 color { material->get_color() };
	ComponentUI::draw_color_control("Color", color);
	material->set_color(color);
}

}
