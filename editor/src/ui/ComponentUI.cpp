#include <array>
#include <string>

#include "ui/ComponentUI.h"

#include "imgui.h"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Texture2D.h"

namespace {

constexpr ImVec2 k_region_item_spacing { 4.0f, 6.0f };

struct ScopedStyleVar {
	explicit ScopedStyleVar(ImGuiStyleVar idx, const ImVec2& value)
	    : m_count(1) {
		ImGui::PushStyleVar(idx, value);
	}

	~ScopedStyleVar() {
		if (m_count > 0)
			ImGui::PopStyleVar(m_count);
	}

	ScopedStyleVar(const ScopedStyleVar&) = delete;
	ScopedStyleVar& operator=(const ScopedStyleVar&) = delete;

private:
	int m_count;
};

struct ScopedStyleColors {
	explicit ScopedStyleColors(std::initializer_list<std::pair<ImGuiCol, ImVec4>> colors)
	    : m_count(static_cast<int>(colors.size())) {
		for (const auto& [idx, color] : colors)
			ImGui::PushStyleColor(idx, color);
	}

	~ScopedStyleColors() {
		if (m_count > 0)
			ImGui::PopStyleColor(m_count);
	}

	ScopedStyleColors(const ScopedStyleColors&) = delete;
	ScopedStyleColors& operator=(const ScopedStyleColors&) = delete;

private:
	int m_count;
};

struct ScopedControlRow {
	explicit ScopedControlRow(std::string_view label)
	    : m_label(label)
	    , m_item_spacing(ImGuiStyleVar_ItemSpacing, ComponentUI::detail::k_control_item_spacing) {
		ImGui::PushID(label.data());
	}

	~ScopedControlRow() {
		ImGui::PopID();
	}

	ScopedControlRow(const ScopedControlRow&) = delete;
	ScopedControlRow& operator=(const ScopedControlRow&) = delete;

	void draw_label() const {
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(m_label.data());
	}

	void align_value(float width) const {
		ImGui::SameLine(ComponentUI::detail::get_control_row_offset(m_label, width));
	}

private:
	std::string_view m_label;
	ScopedStyleVar m_item_spacing;
};

const ImVec4& style_color(ImGuiCol color) {
	return ImGui::GetStyle().Colors[color];
}

bool begin_inspector_region(const char* label, bool default_open = true) {
	ScopedStyleVar frame_padding(ImGuiStyleVar_FramePadding, ImVec2 { 6.0f, 4.0f });
	ScopedStyleColors colors({
	    { ImGuiCol_Header, style_color(ImGuiCol_Header) },
	    { ImGuiCol_HeaderHovered, style_color(ImGuiCol_HeaderHovered) },
	    { ImGuiCol_HeaderActive, style_color(ImGuiCol_HeaderActive) },
	});

	return ImGui::CollapsingHeader(label, default_open ? ImGuiTreeNodeFlags_DefaultOpen : 0);
}

template <typename DrawFn>
void draw_styled_frame(DrawFn&& draw_fn) {
	ScopedStyleColors colors({
	    { ImGuiCol_FrameBg, style_color(ImGuiCol_FrameBg) },
	    { ImGuiCol_FrameBgHovered, style_color(ImGuiCol_FrameBgHovered) },
	    { ImGuiCol_FrameBgActive, style_color(ImGuiCol_FrameBgActive) },
	    { ImGuiCol_Text, style_color(ImGuiCol_Text) },
	});

	draw_fn();
}

template <typename DrawFn>
void draw_styled_checkbox(DrawFn&& draw_fn) {
	ScopedStyleColors colors({
	    { ImGuiCol_FrameBg, style_color(ImGuiCol_FrameBg) },
	    { ImGuiCol_FrameBgHovered, style_color(ImGuiCol_FrameBgHovered) },
	    { ImGuiCol_FrameBgActive, style_color(ImGuiCol_FrameBgActive) },
	    { ImGuiCol_CheckMark, style_color(ImGuiCol_CheckMark) },
	});

	draw_fn();
}

template <typename DrawFn>
void draw_row(std::string_view label, float width, DrawFn&& draw_fn) {
	ScopedControlRow row(label);
	row.draw_label();
	row.align_value(width);
	draw_fn();
}

template <typename DrawFn>
void draw_scalar_control(std::string_view label, float width, DrawFn&& draw_fn) {
	draw_row(label, width, [&] {
		draw_styled_frame([&] {
			ImGui::SetNextItemWidth(width);
			draw_fn();
		});
	});
}

template <size_t N, typename DrawFieldFn>
void draw_multi_float_control(
    std::string_view label,
    const std::array<float*, N>& components,
    const std::array<const char*, N>& ids,
    float field_width,
    float speed,
    DrawFieldFn&& draw_field_fn) {
	constexpr float spacing = 4.0f;
	const float total_width = field_width * static_cast<float>(N) + spacing * static_cast<float>(N - 1);

	draw_row(label, total_width, [&] {
		for (size_t i = 0; i < N; ++i) {
			draw_styled_frame([&] {
				ImGui::SetNextItemWidth(field_width);
				draw_field_fn(ids[i], *components[i], speed);
			});

			if (i + 1 < N)
				ImGui::SameLine(0.0f, spacing);
		}
	});
}

std::string truncate_text(const std::string& text, size_t max_chars) {
	if (text.size() <= max_chars)
		return text;
	return text.substr(0, max_chars - 3) + "...";
}

bool is_texture_asset_field(const Origo::OptionalAssetHandle& handle, const Origo::AssetMetadata& metadata, std::optional<Origo::AssetType> validation_type) {
	return (validation_type && *validation_type == Origo::AssetType::Texture2D)
	    || (handle && metadata.Type == Origo::AssetType::Texture2D);
}

ImTextureID get_texture_preview_id(Origo::AssetManager& asset_manager, const Origo::OptionalAssetHandle& handle, const Origo::AssetMetadata& metadata) {
	if (!handle || metadata.Type != Origo::AssetType::Texture2D)
		return 0;

	if (auto* texture = asset_manager.get_asset<Origo::Texture2D>(*handle))
		return static_cast<ImTextureID>(texture->get_renderer_id());

	return 0;
}

void draw_texture_asset_preview(const std::string& display_name, ImTextureID preview_id, float width) {
	constexpr float padding = 6.0f;
	constexpr float height = 82.0f;
	const float preview_size = height - padding * 2.0f;

	ImGui::InvisibleButton("##TextureAssetField", ImVec2(width, height));

	const ImVec2 card_min = ImGui::GetItemRectMin();
	const ImVec2 card_max = ImGui::GetItemRectMax();
	const ImVec2 preview_min { card_min.x + padding, card_min.y + padding };
	const ImVec2 preview_max { preview_min.x + preview_size, preview_min.y + preview_size };

	const ImU32 bg_color = ImGui::GetColorU32(ImGui::IsItemHovered() ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
	const ImU32 border_color = ImGui::GetColorU32(ImGuiCol_Border);
	const ImU32 text_color = ImGui::GetColorU32(ImGuiCol_Text);
	const ImU32 dim_text_color = ImGui::GetColorU32(ImGuiCol_TextDisabled);
	const ImU32 preview_bg_color = ImGui::GetColorU32(ImGuiCol_ChildBg);

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddRectFilled(card_min, card_max, bg_color, 4.0f);
	draw_list->AddRect(card_min, card_max, border_color, 4.0f);

	if (preview_id) {
		// Rotate preview by 180 degrees to compensate for default flipped orientation.
		draw_list->AddImage(preview_id, preview_min, preview_max, ImVec2(1.0f, 1.0f), ImVec2(0.0f, 0.0f));
	} else {
		draw_list->AddRectFilled(preview_min, preview_max, preview_bg_color, 2.0f);
		draw_list->AddText(ImVec2(preview_min.x + 10.0f, preview_min.y + preview_size * 0.42f), dim_text_color, "No Tex");
	}

	const ImVec2 text_pos { preview_max.x + 8.0f, card_min.y + 10.0f };
	const std::string title = truncate_text(display_name, 20);
	draw_list->AddText(text_pos, text_color, title.c_str());
	draw_list->AddText(ImVec2(text_pos.x, text_pos.y + 22.0f), dim_text_color, "Texture");
}

} // namespace

namespace ComponentUI {

void draw_vec3_control(std::string_view label, glm::vec3& values, float speed) {
	draw_multi_float_control<3>(
	    label,
	    { &values.x, &values.y, &values.z },
	    { "##X", "##Y", "##Z" },
	    65.0f,
	    speed,
	    [](const char* id, float& value, float drag_speed) {
		    ImGui::DragFloat(id, &value, drag_speed, 0.0f, 0.0f, "%.2f");
	    });
}

void draw_min_max_range_control(std::string_view label, glm::vec2& values, float speed) {
	const glm::vec2 original = values;

	draw_multi_float_control<2>(
	    label,
	    { &values.x, &values.y },
	    { "##X", "##Y" },
	    65.0f,
	    speed,
	    [](const char* id, float& value, float drag_speed) {
		    ImGui::DragFloat(id, &value, drag_speed, 0.0f, 0.0f, "%.2f");
	    });

	if (values.x > values.y)
		values = original;
}

void draw_float_control(std::string_view label, float& value, float speed) {
	draw_scalar_control(label, 100.0f, [&] {
		ImGui::DragFloat("##float", &value, speed, 0.0f, 0.0f, "%.3f");
	});
}

void draw_float_control(std::string_view label, float& value, float speed, const char* format) {
	draw_scalar_control(label, 100.0f, [&] {
		ImGui::DragFloat("##float", &value, speed, 0.0f, 0.0f, format);
	});
}

void draw_float_slider_control(std::string_view label, float& value, float min, float max, const char* format) {
	draw_scalar_control(label, 140.0f, [&] {
		ImGui::SliderFloat("##slider", &value, min, max, format);
	});
}

void draw_int_control(std::string_view label, int& value, float speed) {
	draw_scalar_control(label, 90.0f, [&] {
		ImGui::DragInt("##int", &value, speed);
	});
}

void draw_string_control(std::string_view label, std::string& value) {
	draw_scalar_control(label, 180.0f, [&] {
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
	});
}

void draw_bool_control(std::string_view label, bool& value) {
	const float checkbox_width = ImGui::GetFrameHeight();

	draw_row(label, checkbox_width, [&] {
		draw_styled_checkbox([&] {
			ImGui::Checkbox("##bool", &value);
		});
	});
}

void draw_asset_control(std::string_view label, Origo::OptionalAssetHandle& handle, std::optional<Origo::AssetType> assetValidationType) {
	auto& asset_manager = Origo::AssetManager::get_instance();

	const auto uuid = handle ? asset_manager.get_uuid(*handle) : std::nullopt;
	const Origo::AssetMetadata metadata = uuid
	    ? Origo::AssetDatabase::get_instance().get_metadata(*uuid)
	    : Origo::AssetMetadata {};
	const std::string display_name = handle ? metadata.Name : "None";
	const bool texture_field = is_texture_asset_field(handle, metadata, assetValidationType);
	const float field_width = 180.0f;

	draw_row(label, field_width, [&] {
		draw_styled_frame([&] {
			if (texture_field) {
				draw_texture_asset_preview(display_name, get_texture_preview_id(asset_manager, handle, metadata), field_width);
			} else {
				ImGui::Button(display_name.c_str(), ImVec2(field_width, ImGui::GetFrameHeight()));
			}
		});

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ORIGO_ASSET_UUID")) {
				const char* uuid_str = static_cast<const char*>(payload->Data);
				const Origo::UUID payload_id = Origo::UUID::from_string(uuid_str);
				const auto payload_metadata = Origo::AssetDatabase::get_instance().get_metadata(payload_id);

				if (!assetValidationType || Origo::is_asset_type_compatible(payload_metadata.Type, *assetValidationType))
					handle = asset_manager.get_handle_by_uuid(payload_id);
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Clear"))
				handle = {};
			ImGui::EndPopup();
		}
	});
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

	const bool open = begin_inspector_region(label.data(), defaultOpen);

	if (open)
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, k_region_item_spacing);

	return open;
}

void end_region(bool open) {
	if (open)
		ImGui::PopStyleVar();

	ImGui::PopID();
}

void draw_vec2_control(std::string_view label, glm::vec2& values, float speed) {
	draw_multi_float_control<2>(
	    label,
	    { &values.x, &values.y },
	    { "##X", "##Y" },
	    65.0f,
	    speed,
	    [](const char* id, float& value, float drag_speed) {
		    ImGui::DragFloat(id, &value, drag_speed, 0.0f, 0.0f, "%.2f");
	    });
}

} // namespace ComponentUI
