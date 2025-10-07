#include "origo/EventSystem.h"
#include "origo/ScreenWindow.h"

void EventSystem::OnFrameBufferSizeCallback(GLFWwindow* window, int w, int h) {
	auto* es { static_cast<EventSystem*>(glfwGetWindowUserPointer(window)) };

	es->m_ScreenWindow.SetHeight(h);
	es->m_ScreenWindow.SetWidth(w);

	glViewport(0, 0, w, h);

	if (es->m_ResizeCallback) {
		es->m_ResizeCallback(w, h);
	}
}

void EventSystem::MouseMoveDispatcher(GLFWwindow* window, double x, double y) {
	auto* es = static_cast<EventSystem*>(glfwGetWindowUserPointer(window));
	if (es && es->m_MouseMoveCallback) {
		es->m_MouseMoveCallback(x, y);
	}
}

EventSystem::EventSystem(ScreenWindow& window)
    : m_ScreenWindow(window)
    , m_ResizeCallback()
    , m_KeyCallbacks() {
	glfwSetWindowUserPointer(m_ScreenWindow.m_Window, this);
	glfwSetFramebufferSizeCallback(m_ScreenWindow.m_Window, EventSystem::OnFrameBufferSizeCallback);
	glfwSetCursorPosCallback(m_ScreenWindow.m_Window, EventSystem::MouseMoveDispatcher);
}

void EventSystem::OnMouseMove(MouseMoveCallback cb) {
	m_MouseMoveCallback = cb;
}

void EventSystem::OnResize(const EventSystem::ResizeCallback& callback) {
	m_ResizeCallback = callback;
}

void EventSystem::OnKeyCallback(int key, const KeyCallback& cb) {
	m_KeyCallbacks.emplace(key, cb);
}

void EventSystem::CheckEvents() {
	glfwPollEvents();
	for (const auto& [key, cb] : m_KeyCallbacks)
		if (glfwGetKey(m_ScreenWindow.m_Window, key) == GLFW_PRESS)
			cb();
}
