#pragma once

#include <deque>
#include <string>
#include <string_view>

#include "origo/utils/Singleton.h"

#include "ui/EditorPalette.h"

namespace OrigoEditor {

enum class EditorNotificationType {
	Info,
	Success,
	Warning,
	Error
};

struct EditorNotification {
	std::string Title;
	std::string Message;
	EditorNotificationType Type { EditorNotificationType::Info };
	float DurationSeconds { 4.0f };
	float AgeSeconds { 0.0f };
};

class EditorNotificationSystem : public Origo::Singleton<EditorNotificationSystem> {
	friend class Origo::Singleton<EditorNotificationSystem>;

public:
	void notify(std::string_view title, std::string_view message, EditorNotificationType type = EditorNotificationType::Info, float duration_seconds = 4.0f);

	void clear();
	void render(const EditorPalette& palette);

	void info(std::string_view title, std::string_view message, float duration_seconds = 4.0f);
	void success(std::string_view title, std::string_view message, float duration_seconds = 4.0f);
	void warning(std::string_view title, std::string_view message, float duration_seconds = 4.5f);
	void error(std::string_view title, std::string_view message, float duration_seconds = 5.0f);

private:
	EditorNotificationSystem() = default;

	static constexpr float s_card_width = 380.0f;
	static constexpr float s_card_spacing = 12.0f;
	static constexpr float s_screen_margin = 20.0f;
	static constexpr float s_corner_rounding = 0.0f;

	std::deque<EditorNotification> m_notifications;
};

}
