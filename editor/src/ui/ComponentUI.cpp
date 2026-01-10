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
	const float spacing = 4.0f;

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data());

	float totalWidth = fieldWidth * 3.0f + spacing * 2.0f;
	float avail = ImGui::GetContentRegionAvail().x;
	float nextX = ImGui::GetCursorPosX() + avail - totalWidth;
	float minX = ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f;
	if (nextX < minX)
		nextX = minX;

	ImGui::SameLine(nextX);

	auto DrawField = [&](const char* id, float& v) {
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

		ImGui::SetNextItemWidth(fieldWidth);
		ImGui::DragFloat(id, &v, speed, 0.0f, 0.0f, "%.2f");

		ImGui::PopStyleColor(4);
	};

	DrawField("##X", values.x);
	ImGui::SameLine(0.0f, spacing);
	DrawField("##Y", values.y);
	ImGui::SameLine(0.0f, spacing);
	DrawField("##Z", values.z);

	ImGui::PopStyleVar();
	ImGui::PopID();
}

void DrawFloatControl(std::string_view label, float& value, float speed) {
	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float fieldWidth = 100.0f;

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

	ImGui::SetNextItemWidth(fieldWidth);
	ImGui::DragFloat("##float", &value, speed, 0.0f, 0.0f, "%.3f");

	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar();
	ImGui::PopID();
}

void DrawIntControl(std::string_view label, int& value, float speed) {
	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float fieldWidth = 90.0f;

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

	ImGui::SetNextItemWidth(fieldWidth);
	ImGui::DragInt("##int", &value, speed);

	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar();
	ImGui::PopID();
}

void DrawStringControl(std::string_view label, std::string& value) {
	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float fieldWidth = 180.0f;

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

	ImGui::SetNextItemWidth(fieldWidth);
	ImGui::InputText(
	    "##string",
	    value.data(),
	    value.capacity() + 1,
	    ImGuiInputTextFlags_CallbackResize,
	    [](ImGuiInputTextCallbackData* data) {
		    auto* str = static_cast<std::string*>(data->UserData);
		    str->resize(data->BufTextLen);
		    data->Buf = str->data();
		    return 0;
	    },
	    &value);

	ImGui::PopStyleColor(4);
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

void DrawAssetControl(std::string_view label, Origo::OptionalAssetHandle& handle, std::optional<Origo::AssetType> assetValidationType) {
	auto& am = Origo::AssetManager::GetInstance();

	auto uuid = handle ? am.GetUUID(*handle) : std::nullopt;
	auto md = uuid ? Origo::AssetDatabase::GetMetadata(*uuid) : Origo::AssetMetadata {};

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

	std::string display = handle ? md.Name : "None";
	ImGui::Button(display.c_str(), ImVec2(fieldWidth, fieldHeight));

	ImGui::PopStyleColor(4);

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ORIGO_ASSET_UUID")) {
			const char* uuidStr = static_cast<const char*>(payload->Data);
			Origo::UUID payloadID = Origo::UUID::FromString(uuidStr);
			auto payloadMD = Origo::AssetDatabase::GetMetadata(payloadID);

			if (!assetValidationType || payloadMD.Type == *assetValidationType)
				handle = am.GetHandleByUUID(payloadID);
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

void DrawColorControl(std::string_view label, glm::vec3& value) {
	ImGui::PushID(label.data());

	ImVec4 col(value.r, value.g, value.b, 1.0f);

	if (ImGui::BeginTable("ColorRow", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoPadInnerX)) {
		ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 90.0f);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label.data());

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);

		if (ImGui::ColorButton(
		        "##color",
		        col,
		        ImGuiColorEditFlags_NoTooltip,
		        ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight()))) {
			ImGui::OpenPopup("ColorPicker");
		}

		if (ImGui::BeginPopup("ColorPicker")) {
			float c[3] = { value.r, value.g, value.b };
			if (ImGui::ColorPicker3("##picker", c))
				value = { c[0], c[1], c[2] };
			ImGui::EndPopup();
		}

		ImGui::EndTable();
	}

	ImGui::PopID();
}

}
