#include "ui/ComponentUI.h"
#include "imgui.h"
#include "origo/assets/Asset.h"
#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetManagerFast.h"

namespace ComponentUI {
void DrawVec3Control(std::string_view label, glm::vec3& values, float speed) {
	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float fieldWidth = 65.0f;
	const float spacingBetweenFields = 4.0f;

	ImGui::BeginGroup();

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data());

	float totalFieldWidth = fieldWidth * 3.0f + spacingBetweenFields * 2.0f;

	float rightMargin = 5.0f;
	float avail = ImGui::GetContentRegionAvail().x;
	float nextX = ImGui::GetCursorPosX() + avail - totalFieldWidth - rightMargin;
	if (nextX < ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f)
		nextX = ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f;
	ImGui::SameLine(nextX);

	auto DrawField = [&](const char* id, float& v) {
		ImVec4 textColor(0.85f, 0.85f, 0.85f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, textColor);

		ImGui::SetNextItemWidth(fieldWidth);
		ImGui::DragFloat(std::string("##").append(id).c_str(), &v, speed, 0.0f, 0.0f, "%.2f");

		ImGui::PopStyleColor(4);
	};

	DrawField("X", values.x);
	ImGui::SameLine(0.0f, spacingBetweenFields);
	DrawField("Y", values.y);
	ImGui::SameLine(0.0f, spacingBetweenFields);
	DrawField("Z", values.z);

	ImGui::EndGroup();

	ImGui::PopStyleVar();
	ImGui::PopID();
}

void DrawFloatControl(std::string_view label, float& value, float speed) {
	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float fieldWidth = 100.0f;

	ImGui::BeginGroup();

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data());

	float avail = ImGui::GetContentRegionAvail().x;
	float nextX = ImGui::GetCursorPosX() + avail - fieldWidth;
	if (nextX < ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f)
		nextX = ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f;

	ImGui::SameLine(nextX);

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

	ImGui::SetNextItemWidth(fieldWidth);
	ImGui::DragFloat("##float", &value, speed, 0.0f, 0.0f, "%.3f");

	ImGui::PopStyleColor(4);

	ImGui::EndGroup();
	ImGui::PopStyleVar();
	ImGui::PopID();
}

void DrawIntControl(std::string_view label, int& value, float speed) {
	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float fieldWidth = 90.0f;

	ImGui::BeginGroup();

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data());

	float avail = ImGui::GetContentRegionAvail().x;
	float nextX = ImGui::GetCursorPosX() + avail - fieldWidth;
	if (nextX < ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f)
		nextX = ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f;

	ImGui::SameLine(nextX);

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

	ImGui::SetNextItemWidth(fieldWidth);
	ImGui::DragInt("##int", &value, speed);

	ImGui::PopStyleColor(4);

	ImGui::EndGroup();
	ImGui::PopStyleVar();
	ImGui::PopID();
}

void DrawStringControl(std::string_view label, std::string& value) {
	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float fieldWidth = 180.0f;

	ImGui::BeginGroup();

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data());

	float avail = ImGui::GetContentRegionAvail().x;
	float nextX = ImGui::GetCursorPosX() + avail - fieldWidth;
	if (nextX < ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f)
		nextX = ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f;

	ImGui::SameLine(nextX);

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

	ImGui::SetNextItemWidth(fieldWidth);

	ImGui::InputText(
	    "##string",
	    value.data(),
	    value.capacity() + 1,
	    ImGuiInputTextFlags_CallbackResize,
	    [](ImGuiInputTextCallbackData* data) {
		    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
			    auto* str = static_cast<std::string*>(data->UserData);
			    str->resize(data->BufTextLen);
			    data->Buf = str->data();
		    }
		    return 0;
	    },
	    &value);

	ImGui::PopStyleColor(4);

	ImGui::EndGroup();
	ImGui::PopStyleVar();
	ImGui::PopID();
}

void DrawFloatControl(std::string_view label, float& value, float speed, const char* format) {
	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float fieldWidth = 90.0f;

	ImGui::BeginGroup();

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data());

	float avail = ImGui::GetContentRegionAvail().x;
	float nextX = ImGui::GetCursorPosX() + avail - fieldWidth;
	if (nextX < ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f)
		nextX = ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f;

	ImGui::SameLine(nextX);

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

	ImGui::SetNextItemWidth(fieldWidth);
	ImGui::DragFloat("##float", &value, speed, 0.0f, 0.0f, format);

	ImGui::PopStyleColor(4);

	ImGui::EndGroup();
	ImGui::PopStyleVar();
	ImGui::PopID();
}

void DrawBoolControl(std::string_view label, bool& value) {
	ImGui::PushID(label.data(), label.data() + label.size());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float fieldWidth = 180.0f;

	ImGui::BeginGroup();

	ImVec2 lineStart = ImGui::GetCursorScreenPos();

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data(), label.data() + label.size());

	float right = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

	float labelWidth = ImGui::CalcTextSize(label.data(), label.data() + label.size()).x;

	float fieldX = right - fieldWidth;

	float minX = lineStart.x + labelWidth + 8.0f;
	if (fieldX < minX)
		fieldX = minX;

	float checkboxSize = ImGui::GetFrameHeight();

	ImGui::SameLine();
	ImGui::SetCursorScreenPos(
	    ImVec2(fieldX + fieldWidth - checkboxSize, lineStart.y));

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

	ImGui::Checkbox("##bool", &value);

	ImGui::PopStyleColor(4);

	ImGui::EndGroup();
	ImGui::PopStyleVar();
	ImGui::PopID();
}

void DrawAssetControl(std::string_view label, Origo::AssetHandle& handle, std::optional<Origo::AssetType> assetValidationType) {
	auto& am = Origo::AssetManagerFast::GetInstance();

	Origo::UUID uuid = !handle.IsNull() ? am.GetUUID(handle) : Origo::UUID {};
	auto md = !uuid.IsBad() ? Origo::AssetDatabase::GetMetadata(uuid)
	                        : Origo::AssetMetadata {};

	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float fieldWidth = 180.0f;
	const float fieldHeight = ImGui::GetFrameHeight();

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data());

	float avail = ImGui::GetContentRegionAvail().x;
	float nextX = ImGui::GetCursorPosX() + avail - fieldWidth;
	float minX = ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f;
	if (nextX < minX)
		nextX = minX;

	ImGui::SameLine(nextX);

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

	std::string display = !handle.IsNull() ? md.Name : std::string { "None" };
	ImGui::Button(display.c_str(), ImVec2(fieldWidth, fieldHeight));

	ImGui::PopStyleColor(4);

	ImRect rect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
	ImGuiID id = ImGui::GetID("AssetDropTarget");

	if (ImGui::BeginDragDropTargetCustom(rect, id)) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ORIGO_ASSET_UUID")) {
			const char* uuidStr = static_cast<const char*>(payload->Data);
			Origo::UUID payloadID = Origo::UUID::FromString(uuidStr);
			auto payloadMetadata { Origo::AssetDatabase::GetMetadata(payloadID) };
			bool anyTypeValid { !assetValidationType.has_value() };

			if (anyTypeValid || payloadMetadata.Type == *assetValidationType) {
				handle = am.GetHandleByUUID(payloadID);
			} else {
				if (assetValidationType) {
					std::string assetValidationTypeStr { magic_enum::enum_name(*assetValidationType) };
					std::string providedAssetTypeStr { magic_enum::enum_name(payloadMetadata.Type) };
					ORG_CORE_WARN("Received {} while the asset control {} expects {}", providedAssetTypeStr, label, assetValidationTypeStr);
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Clear"))
			handle = {};
		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();
	ImGui::PopID();
}
}
