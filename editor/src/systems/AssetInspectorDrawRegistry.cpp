#include "origo/assets/Asset.h"
#include "origo/assets/Material.h"
#include "systems/AssetInspectorDrawer.h"
#include "imgui.h"
#include "origo/assets/AssetManagerFast.h"
#include "ui/ComponentUI.h"

namespace OrigoEditor {

void AssetInspectorDrawer::Draw(const Origo::AssetMetadata& md) {
	auto& am { Origo::AssetManagerFast::GetInstance() };
	auto assetPtr { am.Get(am.GetHandleByUUID(md.ID)) };
	DrawSpecific(assetPtr, md.Type);
}

void AssetInspectorDrawer::DrawSpecific(Origo::Asset* asset, Origo::AssetType type) {
	switch (type) {
	case Origo::AssetType::Material:
		DrawMaterial(dynamic_cast<Origo::Material2D*>(asset));
		break;
	default: {
		std::string assetTypeStr { magic_enum::enum_name(type) };
		ORG_CORE_WARN("No draw for assets of type {} implemented yet", assetTypeStr);
		ImGui::Text("%s", ("No draw for assets of type " + assetTypeStr + "implemented yet").c_str());
	}
	}
}

void AssetInspectorDrawer::DrawMaterial(Origo::Material2D* material) {
	auto albedoHandle { material->GetAlbedo() };
	ComponentUI::DrawAssetControl("Albedo", albedoHandle, Origo::AssetType::Texture2D);
	material->SetAlbedo(albedoHandle);
}

}
