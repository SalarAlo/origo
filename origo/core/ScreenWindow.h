#pragma once

#include "ScreenWindowSettings.h"

#include "origo/events/Event.h"

namespace Origo {

class ScreenWindow {
public:
	explicit ScreenWindow(const ScreenWindowSettings& screenWindowConfig);
	~ScreenWindow();

	ScreenWindow(const ScreenWindow& other) = delete;
	ScreenWindow& operator=(const ScreenWindow& other) = delete;
	ScreenWindow(ScreenWindow&& other) = delete;
	ScreenWindow& operator=(ScreenWindow&& other) = delete;

	bool should_close() const;
	void on_update() const;

	void set_width(int width);
	void set_height(int height);

	int get_width() const;
	int get_height() const;

	float get_aspect_resolution() const;
	GLFWwindow* get_native_window() const;

	void set_event_callback(const EventCallbackFn& callback);

private:
	void init_callback();
	static void init_glad();
	static void init_glfw();

private:
	ScreenWindowSettings m_screen_window_settings {};
	GLFWwindow* m_window {};

	static bool s_single_instance_created;
};

}
