#pragma once

#include "ScreenWindowSettings.h"

namespace Origo {

class ScreenWindow {
	friend class EventSystem;

public:
	explicit ScreenWindow(const ScreenWindowSettings& screenWindowConfig);

	bool ShouldClose() const;
	void SwapBuffers() const;

	void SetWidth(int width);
	void SetHeight(int height);

	int GetWidth() const;
	int GetHeight() const;

	float GetAspectResolution() const;

private:
	static void InitGlad();
	static void InitGlfw();

private:
	ScreenWindowSettings m_ScreenWindowSettings {};
	GLFWwindow* m_Window {};

	static bool s_SingleInstanceCreated;
};

}
