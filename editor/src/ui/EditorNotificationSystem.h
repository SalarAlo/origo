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
	float DurationSeconds { 0.8f };
	float AgeSeconds { 0.0f };
};

class EditorNotificationSystem : public Origo::Singleton<EditorNotificationSystem> {
	friend class Origo::Singleton<EditorNotificationSystem>;

public:
	bool is_enabled() const { return m_enabled; }
	void set_enabled();
	void set_disabled();

	void notify(std::string_view title, std::string_view message, EditorNotificationType type = EditorNotificationType::Info);

	void clear();
	void render(const EditorPalette& palette);

	void info(std::string_view title, std::string_view message);
	void success(std::string_view title, std::string_view message);
	void warning(std::string_view title, std::string_view message);
	void error(std::string_view title, std::string_view message);

private:
	EditorNotificationSystem() = default;

	const float m_duration_seconds = 0.8f;
	const float m_card_width = 388.0f;
	const float m_card_spacing = 12.0f;
	const float m_screen_margin = 20.0f;
	const float m_corner_rounding = 18.0f;

	bool m_enabled = true;

	std::deque<EditorNotification>
	    m_notifications;
};

}
