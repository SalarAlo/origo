#include <algorithm>

#include "ui/EditorNotificationSystem.h"

namespace OrigoEditor {

namespace {
	ImVec4 scale_alpha(const ImVec4& color, float alpha) {
		return { color.x, color.y, color.z, color.w * alpha };
	}

	ImVec4 get_type_color(EditorNotificationType type, const EditorPalette& palette) {
		switch (type) {
		case EditorNotificationType::Info:
			return palette.AccentStrong;
		case EditorNotificationType::Success:
			return { 0.25f, 0.73f, 0.48f, 1.0f };
		case EditorNotificationType::Warning:
			return { 0.94f, 0.68f, 0.25f, 1.0f };
		case EditorNotificationType::Error:
			return palette.Error;
		}

		return palette.AccentStrong;
	}

	const char* get_type_label(EditorNotificationType type) {
		switch (type) {
		case EditorNotificationType::Info:
			return "INFO";
		case EditorNotificationType::Success:
			return "SUCCESS";
		case EditorNotificationType::Warning:
			return "WARNING";
		case EditorNotificationType::Error:
			return "ERROR";
		}

		return "INFO";
	}

	float compute_visibility(float age_seconds, float duration_seconds) {
		constexpr float fade_duration = 0.22f;

		if (duration_seconds <= 0.0f)
			return 0.0f;

		const float fade_window = std::min(fade_duration, duration_seconds * 0.5f);
		const float fade_in = std::clamp(age_seconds / fade_window, 0.0f, 1.0f);
		const float fade_out = std::clamp((duration_seconds - age_seconds) / fade_window, 0.0f, 1.0f);
		return std::min(fade_in, fade_out);
	}

}

void EditorNotificationSystem::set_enabled() {
	m_enabled = true;
}

void EditorNotificationSystem::set_disabled() {
	m_enabled = false;
}

void EditorNotificationSystem::notify(
    std::string_view title,
    std::string_view message,
    EditorNotificationType type) {
	if (!m_enabled)
		return;

	m_notifications.emplace_front(EditorNotification {
	    .Title = std::string(title),
	    .Message = std::string(message),
	    .Type = type,
	    .DurationSeconds = m_duration_seconds,
	    .AgeSeconds = 0.0f,
	});
}

void EditorNotificationSystem::clear() {
	m_notifications.clear();
}

void EditorNotificationSystem::info(std::string_view title, std::string_view message) {
	notify(title, message, EditorNotificationType::Info);
}

void EditorNotificationSystem::success(std::string_view title, std::string_view message) {
	notify(title, message, EditorNotificationType::Success);
}

void EditorNotificationSystem::warning(std::string_view title, std::string_view message) {
	notify(title, message, EditorNotificationType::Warning);
}

void EditorNotificationSystem::error(std::string_view title, std::string_view message) {
	notify(title, message, EditorNotificationType::Error);
}

void EditorNotificationSystem::render(const EditorPalette& palette) {
	if (m_notifications.empty())
		return;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	if (viewport == nullptr)
		return;

	ImDrawList* draw_list = ImGui::GetForegroundDrawList(viewport);
	if (draw_list == nullptr)
		return;

	const float dt = ImGui::GetIO().DeltaTime;
	for (EditorNotification& notification : m_notifications)
		notification.AgeSeconds += dt;

	std::erase_if(m_notifications, [](const EditorNotification& notification) {
		return notification.AgeSeconds >= notification.DurationSeconds;
	});

	if (m_notifications.empty())
		return;

	const ImVec2 origin = {
		viewport->WorkPos.x + viewport->WorkSize.x - m_screen_margin - m_card_width,
		viewport->WorkPos.y + m_screen_margin
	};

	const float inner_padding_x = 18.0f;
	const float inner_padding_y = 15.0f;
	const float body_spacing = 7.0f;
	const float badge_height = 24.0f;
	const float badge_width = 84.0f;
	const float accent_pill_width = 6.0f;
	const float accent_gap = 14.0f;
	const float wrap_width = m_card_width - (inner_padding_x * 2.0f) - accent_pill_width - accent_gap - badge_width - 12.0f;

	float cursor_y = origin.y;
	for (const EditorNotification& notification : m_notifications) {
		const float visibility = compute_visibility(notification.AgeSeconds, notification.DurationSeconds);
		if (visibility <= 0.0f)
			continue;

		const ImVec4 accent = get_type_color(notification.Type, palette);
		const ImVec4 title_color = scale_alpha(palette.Text, visibility);
		const ImVec4 message_color = scale_alpha(palette.TextDim, visibility);
		const ImVec4 badge_text_color = scale_alpha(accent, visibility);
		const ImVec4 border_color = scale_alpha(palette.BorderStrong, 0.62f * visibility);
		const ImVec4 shadow_color = { 0.0f, 0.0f, 0.0f, 0.18f * visibility };
		const ImVec4 accent_strip_color = scale_alpha(accent, 0.95f * visibility);

		const ImVec4 card_color = {
			palette.Surface1.x * 1.02f,
			palette.Surface1.y * 1.02f,
			palette.Surface1.z * 1.02f,
			0.98f * visibility
		};

		const ImVec4 badge_color = {
			palette.Surface2.x,
			palette.Surface2.y,
			palette.Surface2.z,
			0.92f * visibility
		};

		const ImVec2 title_size = ImGui::CalcTextSize(notification.Title.c_str(), nullptr, false, wrap_width);
		const ImVec2 message_size = ImGui::CalcTextSize(notification.Message.c_str(), nullptr, false, wrap_width);
		const float content_height = inner_padding_y * 2.0f + title_size.y + body_spacing + message_size.y;
		const float card_height = std::max(content_height, 88.0f);
		const float slide_offset = (1.0f - visibility) * 18.0f;

		const ImVec2 min = { origin.x, cursor_y - slide_offset };
		const ImVec2 max = { origin.x + m_card_width, cursor_y + card_height - slide_offset };
		const ImVec2 shadow_offset = { 0.0f, 7.0f };
		const ImVec2 badge_min = { max.x - inner_padding_x - badge_width, min.y + inner_padding_y };
		const ImVec2 badge_max = { max.x - inner_padding_x, badge_min.y + badge_height };
		const float accent_pill_height = max.y - min.y - (inner_padding_y * 2.0f);
		const ImVec2 accent_pill_min = { min.x + inner_padding_x, min.y + inner_padding_y };
		const ImVec2 accent_pill_max = { accent_pill_min.x + accent_pill_width, accent_pill_min.y + accent_pill_height };
		const ImVec2 title_pos = { accent_pill_max.x + accent_gap, min.y + inner_padding_y + 1.0f };
		const ImVec2 message_pos = { title_pos.x, title_pos.y + title_size.y + body_spacing + 1.0f };

		draw_list->AddRectFilled(
		    min + shadow_offset,
		    max + shadow_offset,
		    ImGui::GetColorU32(shadow_color),
		    m_corner_rounding);

		draw_list->AddRectFilled(
		    min,
		    max,
		    ImGui::GetColorU32(card_color),
		    m_corner_rounding);

		draw_list->AddRect(
		    min,
		    max,
		    ImGui::GetColorU32(border_color),
		    m_corner_rounding,
		    0,
		    1.0f);

		draw_list->AddRectFilled(
		    accent_pill_min,
		    accent_pill_max,
		    ImGui::GetColorU32(accent_strip_color),
		    accent_pill_width * 0.5f);

		draw_list->AddRectFilled(
		    badge_min,
		    badge_max,
		    ImGui::GetColorU32(badge_color),
		    badge_height * 0.5f);

		draw_list->AddRect(
		    badge_min,
		    badge_max,
		    ImGui::GetColorU32(scale_alpha(accent, 0.32f * visibility)),
		    badge_height * 0.5f,
		    0,
		    1.0f);

		const ImVec2 badge_text_size = ImGui::CalcTextSize(get_type_label(notification.Type));
		const ImVec2 badge_text_pos = {
			badge_min.x + ((badge_max.x - badge_min.x) - badge_text_size.x) * 0.5f,
			badge_min.y + ((badge_max.y - badge_min.y) - badge_text_size.y) * 0.5f
		};

		draw_list->AddText(
		    nullptr,
		    0.0f,
		    title_pos,
		    ImGui::GetColorU32(title_color),
		    notification.Title.c_str(),
		    nullptr,
		    wrap_width);

		draw_list->AddText(
		    nullptr,
		    0.0f,
		    message_pos,
		    ImGui::GetColorU32(message_color),
		    notification.Message.c_str(),
		    nullptr,
		    wrap_width);

		draw_list->AddText(
		    badge_text_pos,
		    ImGui::GetColorU32(badge_text_color),
		    get_type_label(notification.Type));

		cursor_y += card_height + m_card_spacing;
	}
}

}
