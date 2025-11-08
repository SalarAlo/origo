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

	bool ShouldClose() const;
	void OnUpdate() const;

	void SetWidth(int width);
	void SetHeight(int height);

	int GetWidth() const;
	int GetHeight() const;

	float GetAspectResolution() const;
	GLFWwindow* GetNativeWindow() const;

	void SetEventCallback(const EventCallbackFn& callback);

private:
	void InitCallback();
	static void InitGlad();
	static void InitGlfw();

private:
	ScreenWindowSettings m_ScreenWindowSettings {};
	GLFWwindow* m_Window {};

	static bool s_SingleInstanceCreated;
};

}
