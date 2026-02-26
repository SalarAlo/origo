#include "ui/ComponentUI.h"

#include "imgui.h"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetManager.h"

static bool begin_inspector_region(const char* label, bool defaultOpen = true) {
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.18f, 0.18f, 0.18f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.24f, 0.24f, 0.24f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.28f, 0.28f, 0.28f, 1.0f));

	bool open = ImGui::CollapsingHeader(label,
	    defaultOpen ? ImGuiTreeNodeFlags_DefaultOpen : 0);

	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();

	return open;
}

namespace ComponentUI {

void draw_vec3_control(std::string_view label, glm::vec3& values, float speed) {
	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float field_width = 65.0f;
	const float spacing = 4.0f;

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data());

	float total_width = field_width * 3.0f + spacing * 2.0f;
	float avail = ImGui::GetContentRegionAvail().x;
	float next_x = ImGui::GetCursorPosX() + avail - total_width;
	float min_x = ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f;
	if (next_x < min_x)
		next_x = min_x;

	ImGui::SameLine(next_x);

	auto draw_field = [&](const char* id, float& v) {
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

		ImGui::SetNextItemWidth(field_width);
		ImGui::DragFloat(id, &v, speed, 0.0f, 0.0f, "%.2f");

		ImGui::PopStyleColor(4);
	};

	draw_field("##X", values.x);
	ImGui::SameLine(0.0f, spacing);
	draw_field("##Y", values.y);
	ImGui::SameLine(0.0f, spacing);
	draw_field("##Z", values.z);

	ImGui::PopStyleVar();
	ImGui::PopID();
}

void draw_min_max_range_control(std::string_view label, glm::vec2& values, float speed) {
	glm::vec2 original { values };

	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float field_width = 65.0f;
	const float spacing = 4.0f;

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data());

	float total_width = field_width * 2.0f + spacing;
	float avail = ImGui::GetContentRegionAvail().x;
	float next_x = ImGui::GetCursorPosX() + avail - total_width;
	float min_x = ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f;

	if (next_x < min_x)
		next_x = min_x;

	ImGui::SameLine(next_x);

	auto draw_field = [&](const char* id, float& v) {
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

		ImGui::SetNextItemWidth(field_width);
		ImGui::DragFloat(id, &v, speed, 0.0f, 0.0f, "%.2f");

		ImGui::PopStyleColor(4);
	};

	draw_field("##X", values.x);
	ImGui::SameLine(0.0f, spacing);
	draw_field("##Y", values.y);

	if (values.x > values.y) {
		values = original;
	} else if (values.y < values.x) {
		values = original;
	}

	ImGui::PopStyleVar();
	ImGui::PopID();
}

void draw_float_control(std::string_view label, float& value, float speed) {
	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float field_width = 100.0f;

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data());

	float avail = ImGui::GetContentRegionAvail().x;
	float next_x = ImGui::GetCursorPosX() + avail - field_width;
	float min_x = ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f;
	if (next_x < min_x)
		next_x = min_x;

	ImGui::SameLine(next_x);

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

	ImGui::SetNextItemWidth(field_width);
	ImGui::DragFloat("##float", &value, speed, 0.0f, 0.0f, "%.3f");

	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar();
	ImGui::PopID();
}

void draw_int_control(std::string_view label, int& value, float speed) {
	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float field_width = 90.0f;

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data());

	float avail = ImGui::GetContentRegionAvail().x;
	float next_x = ImGui::GetCursorPosX() + avail - field_width;
	float min_x = ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f;
	if (next_x < min_x)
		next_x = min_x;

	ImGui::SameLine(next_x);

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

	ImGui::SetNextItemWidth(field_width);
	ImGui::DragInt("##int", &value, speed);

	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar();
	ImGui::PopID();
}

void draw_string_control(std::string_view label, std::string& value) {
	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float field_width = 180.0f;

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data());

	float avail = ImGui::GetContentRegionAvail().x;
	float next_x = ImGui::GetCursorPosX() + avail - field_width;
	float min_x = ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f;
	if (next_x < min_x)
		next_x = min_x;

	ImGui::SameLine(next_x);

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

	ImGui::SetNextItemWidth(field_width);
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

void draw_bool_control(std::string_view label, bool& value) {
	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data());

	float checkbox_size = ImGui::GetFrameHeight();

	float avail = ImGui::GetContentRegionAvail().x;
	float next_x = ImGui::GetCursorPosX() + avail - checkbox_size;

	float min_x = ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f;
	if (next_x < min_x)
		next_x = min_x;

	ImGui::SameLine(next_x);

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

	ImGui::Checkbox("##bool", &value);

	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar();
	ImGui::PopID();
}

void draw_asset_control(std::string_view label, Origo::OptionalAssetHandle& handle, std::optional<Origo::AssetType> assetValidationType) {
	auto& am = Origo::AssetManager::get_instance();

	auto uuid = handle ? am.get_uuid(*handle) : std::nullopt;
	auto md = uuid ? Origo::AssetDatabase::get_instance().get_metadata(*uuid) : Origo::AssetMetadata {};

	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float field_width = 180.0f;
	const float field_height = ImGui::GetFrameHeight();

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data());

	float avail = ImGui::GetContentRegionAvail().x;
	float next_x = ImGui::GetCursorPosX() + avail - field_width;
	float min_x = ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f;
	if (next_x < min_x)
		next_x = min_x;

	ImGui::SameLine(next_x);

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

	std::string display = handle ? md.Name : "None";
	ImGui::Button(display.c_str(), ImVec2(field_width, field_height));

	ImGui::PopStyleColor(4);

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ORIGO_ASSET_UUID")) {
			const char* uuid_str = static_cast<const char*>(payload->Data);
			Origo::UUID payload_id = Origo::UUID::from_string(uuid_str);
			auto payload_md = Origo::AssetDatabase::get_instance().get_metadata(payload_id);

			if (!assetValidationType || payload_md.Type == *assetValidationType)
				handle = am.get_handle_by_uuid(payload_id);
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

void draw_color_control(std::string_view label, glm::vec3& value) {
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

bool start_region(std::string_view label, bool defaultOpen) {
	ImGui::PushID(label.data());

	bool open = begin_inspector_region(label.data(), defaultOpen);

	if (open) {
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 6.0f });
	}

	return open;
}

void end_region(bool open) {
	if (open) {
		ImGui::PopStyleVar();
	}

	ImGui::PopID();
}

void draw_vec2_control(std::string_view label, glm::vec2& values, float speed) {
	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float field_width = 65.0f;
	const float spacing = 4.0f;

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data());

	float total_width = field_width * 2.0f + spacing;
	float avail = ImGui::GetContentRegionAvail().x;
	float next_x = ImGui::GetCursorPosX() + avail - total_width;
	float min_x = ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f;

	if (next_x < min_x)
		next_x = min_x;

	ImGui::SameLine(next_x);

	auto draw_field = [&](const char* id, float& v) {
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

		ImGui::SetNextItemWidth(field_width);
		ImGui::DragFloat(id, &v, speed, 0.0f, 0.0f, "%.2f");

		ImGui::PopStyleColor(4);
	};

	draw_field("##X", values.x);
	ImGui::SameLine(0.0f, spacing);
	draw_field("##Y", values.y);

	ImGui::PopStyleVar();
	ImGui::PopID();
}
}
