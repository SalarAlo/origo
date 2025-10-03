#pragma once

#include "ScreenWindowSettings.h"

class ScreenWindow {
public:
	static void InitGlfw();

	explicit ScreenWindow(const ScreenWindowSettings& screenWindowConfig);

	bool ShouldClose() const;
	void SwapBuffers() const;

	void SetWidth(int width);
	void SetHeight(int height);

private:
	static void InitGlad();

private:
	ScreenWindowSettings m_ScreenWindowSettings {};
	GLFWwindow* m_Window {};

	static bool s_SingleInstanceCreated;
};
