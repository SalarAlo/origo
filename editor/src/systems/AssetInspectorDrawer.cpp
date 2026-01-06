#include "origo/assets/Asset.h"
#include "origo/assets/Material.h"
#include "systems/AssetInspectorDrawer.h"
#include "imgui.h"
#include "origo/assets/AssetManagerFast.h"
#include "ui/ComponentUI.h"

namespace OrigoEditor {

void AssetInspectorDrawer::Draw(const Origo::AssetMetadata& md) {
	auto& am { Origo::AssetManager::GetInstance() };
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
		ImGui::Text("%s", ("No draw for assets of type " + assetTypeStr + "implemented yet").c_str());
	}
	}
}

void AssetInspectorDrawer::DrawMaterial(Origo::Material2D* material) {
	auto albedoHandle { material->GetAlbedo() };
	auto shaderHandle { material->GetShader() };

	ComponentUI::DrawAssetControl("Albedo", albedoHandle, Origo::AssetType::Texture2D);
	ComponentUI::DrawAssetControl("Shader", shaderHandle, Origo::AssetType::Shader);

	material->SetAlbedo(albedoHandle);
	material->SetShader(shaderHandle);
}

}
