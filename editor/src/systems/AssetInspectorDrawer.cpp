#include "systems/AssetInspectorDrawer.h"

#include "imgui.h"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"

#include "origo/assets/material/MaterialColor.h"
#include "origo/assets/material/MaterialPBR.h"
#include "origo/assets/material/MaterialPBRSource.h"

#include "panels/MaterialThumbnailRenderer.h"

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
	case Origo::AssetType::MaterialPBR:
		draw_material_pbr(dynamic_cast<Origo::MaterialPBR*>(asset));
		break;
	case Origo::AssetType::MaterialColor:
		draw_material_color(dynamic_cast<Origo::MaterialColor*>(asset));
		break;
	default: {
		std::string asset_type_str { magic_enum::enum_name(type) };
		ImGui::Text("%s", ("No draw for assets of type " + asset_type_str + "implemented yet").c_str());
	}
	}
}

void AssetInspectorDrawer::draw_material_pbr(Origo::MaterialPBR* material) {
	if (!material)
		return;

	auto& material_data { material->get_data() };

	Origo::OptionalAssetHandle albedo_handle = material_data.PBRTexs.Albedo;
	Origo::OptionalAssetHandle normal_handle = material_data.PBRTexs.Normal;
	Origo::OptionalAssetHandle metallic_roughness_handle = material_data.PBRTexs.MetallicRoughness;
	Origo::OptionalAssetHandle ao_handle = material_data.PBRTexs.Ao;
	Origo::OptionalAssetHandle emissive_handle = material_data.PBRTexs.Emissive;
	Origo::OptionalAssetHandle shader_handle = material->get_shader_handle();

	bool textures_open = ComponentUI::start_region("Texture Maps", true);
	if (textures_open) {
		ComponentUI::draw_asset_control("Albedo", albedo_handle, Origo::AssetType::Texture2D);
		ComponentUI::draw_asset_control("Normal", normal_handle, Origo::AssetType::Texture2D);
		ComponentUI::draw_asset_control("Metallic Roughness", metallic_roughness_handle, Origo::AssetType::Texture2D);
		ComponentUI::draw_asset_control("AO", ao_handle, Origo::AssetType::Texture2D);
		ComponentUI::draw_asset_control("Emissive", emissive_handle, Origo::AssetType::Texture2D);
	}
	ComponentUI::end_region(textures_open);

	Origo::Vec3 color { material_data.PBRParams.BaseColor };
	float metallic { material_data.PBRParams.Metallic };
	float roughness { material_data.PBRParams.Roughness };
	float ao { material_data.PBRParams.AO };
	const auto previous_albedo = material_data.PBRTexs.Albedo;
	const auto previous_normal = material_data.PBRTexs.Normal;
	const auto previous_metallic_roughness = material_data.PBRTexs.MetallicRoughness;
	const auto previous_ao = material_data.PBRTexs.Ao;
	const auto previous_emissive = material_data.PBRTexs.Emissive;
	const auto previous_shader = material->get_shader_handle();
	const auto previous_color = material_data.PBRParams.BaseColor;
	const auto previous_metallic = material_data.PBRParams.Metallic;
	const auto previous_roughness = material_data.PBRParams.Roughness;
	const auto previous_ao_value = material_data.PBRParams.AO;

	bool surface_open = ComponentUI::start_region("Surface", true);
	if (surface_open) {
		ComponentUI::draw_color_control("Color", color);
		ComponentUI::draw_float_slider_control("Metallic", metallic, 0.0f, 1.0f);
		ComponentUI::draw_float_slider_control("Roughness", roughness, 0.0f, 1.0f);
		ComponentUI::draw_float_slider_control("AO", ao, 0.0f, 1.0f);
	}
	ComponentUI::end_region(surface_open);

	bool shader_open = ComponentUI::start_region("Shader", false);
	if (shader_open) {
		ComponentUI::draw_asset_control("Shader", shader_handle, Origo::AssetType::Shader);
	}
	ComponentUI::end_region(shader_open);

	material->set_albedo(albedo_handle).set_shader(shader_handle);
	material_data.PBRTexs.Normal = normal_handle;
	material_data.PBRTexs.MetallicRoughness = metallic_roughness_handle;
	material_data.PBRTexs.Ao = ao_handle;
	material_data.PBRTexs.Emissive = emissive_handle;

	if (material_data.PBRParams.BaseColor != color)
		material_data.PBRParams.BaseColor = color;
	if (material_data.PBRParams.Metallic != metallic)
		material_data.PBRParams.Metallic = metallic;
	if (material_data.PBRParams.Roughness != roughness)
		material_data.PBRParams.Roughness = roughness;
	if (material_data.PBRParams.AO != ao)
		material_data.PBRParams.AO = ao;

	if (previous_albedo != material_data.PBRTexs.Albedo
	    || previous_normal != material_data.PBRTexs.Normal
	    || previous_metallic_roughness != material_data.PBRTexs.MetallicRoughness
	    || previous_ao != material_data.PBRTexs.Ao
	    || previous_emissive != material_data.PBRTexs.Emissive
	    || previous_shader != material->get_shader_handle()
	    || previous_color != material_data.PBRParams.BaseColor
	    || previous_metallic != material_data.PBRParams.Metallic
	    || previous_roughness != material_data.PBRParams.Roughness
	    || previous_ao_value != material_data.PBRParams.AO) {
		material->resolve();
		if (auto material_id = Origo::AssetManager::get_instance().get_uuid(material))
			MaterialThumbnailRenderer::get_instance().invalidate(*material_id);
	}
}

void AssetInspectorDrawer::draw_material_color(Origo::MaterialColor* material) {
	auto& material_data { material->get_data() };
	Origo::OptionalAssetHandle shader_handle = material->get_shader_handle();
	bool unlit = material_data.Unlit;

	bool surface_open = ComponentUI::start_region("Surface", true);
	if (surface_open)
		ComponentUI::draw_bool_control("Unlit", unlit);
	ComponentUI::end_region(surface_open);

	bool shader_open = ComponentUI::start_region("Shader", false);
	if (shader_open)
		ComponentUI::draw_asset_control("Shader", shader_handle, Origo::AssetType::Shader);
	ComponentUI::end_region(shader_open);

	material->set_shader(shader_handle);
	material_data.Unlit = unlit;
}

}
