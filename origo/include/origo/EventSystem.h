#pragma once

#include <unordered_map>

#include "ScreenWindow.h"

class EventSystem {
public:
	using ResizeCallback = std::function<void(int, int)>;
	using KeyCallback = std::function<void()>;
	using MouseMoveCallback = std::function<void(double, double)>;

public:
	explicit EventSystem(ScreenWindow& window);

	void OnResize(const ResizeCallback& cb);
	void OnKeyCallback(int key, const KeyCallback& cb);
	void OnMouseMove(MouseMoveCallback cb);

	void CheckEvents();

	static void OnFrameBufferSizeCallback(GLFWwindow* window, int w, int h);
	static void MouseMoveDispatcher(GLFWwindow* window, double x, double y);

private:
	ScreenWindow& m_ScreenWindow;
	ResizeCallback m_ResizeCallback {};
	MouseMoveCallback m_MouseMoveCallback {};
	std::unordered_map<int, KeyCallback> m_KeyCallbacks {};
};
