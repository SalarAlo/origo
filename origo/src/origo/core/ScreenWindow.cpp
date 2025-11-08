#include "origo/core/ScreenWindow.h"
#include "origo/events/Event.h"
#include "origo/events/EventTypes.h"
#include "origo/events/KeyEvent.h"
#include "origo/events/MouseEvent.h"
#include "origo/events/WindowEvent.h"
#include "origo/input/Input.h"
#include "origo/renderer/GlDebug.h"

#include <cassert>
#include <stdexcept>

namespace Origo {
bool ScreenWindow::s_SingleInstanceCreated { false };

ScreenWindow::~ScreenWindow() {
	ORG_CORE_INFO("[ScreenWindow] Destroying a Screen Window");
	glfwTerminate();
}

void ScreenWindow::InitGlfw() {
	if (!glfwInit()) {
		ORG_CORE_ERROR("[ScreenWindow] Failed to initialize GLFW");
		throw std::runtime_error("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
}

void ScreenWindow::InitGlad() {
	if (!glfwGetCurrentContext()) {
		ORG_CORE_ERROR("[ScreenWindow] No active OpenGL context before GLAD init!");
		throw std::runtime_error("No active context");
	}

	bool ok = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	if (!ok) {
		ORG_CORE_ERROR("[ScreenWindow] Failed to initialize OpenGL");
		throw std::runtime_error("Failed to initialize OpenGL");
	}

	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glDepthFunc(GL_LESS));
	GLCall(glClearColor(0, 0, 0, 1));
}

ScreenWindow::ScreenWindow(const ScreenWindowSettings& screenWindowConfig)
    : m_ScreenWindowSettings(screenWindowConfig) {
	if (s_SingleInstanceCreated) {
		ORG_CORE_ERROR("[ScreenWindow] There can only be one instance at a time");
		throw std::runtime_error("There can only be one instance at a time");
	}

	InitGlfw();
	m_Window = glfwCreateWindow(m_ScreenWindowSettings.Width, m_ScreenWindowSettings.Height, m_ScreenWindowSettings.Title.c_str(), nullptr, nullptr);

	glfwMakeContextCurrent(m_Window);
	glfwSetWindowUserPointer(m_Window, &m_ScreenWindowSettings);

	InitGlad();

	Input::SetContext(this);
	Input::SetCursorMode(Input::CursorMode::Locked);

	glfwSwapInterval(1);

	InitCallback();

	GLCall(glViewport(0, 0, screenWindowConfig.Width, screenWindowConfig.Height));

	s_SingleInstanceCreated = true;
}

void ScreenWindow::InitCallback() {
#pragma region MOUSE_MOVE
	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
		auto windowSettings { static_cast<ScreenWindowSettings*>(glfwGetWindowUserPointer(window)) };
		MouseMoveEvent mouseMoveEvent { glm::vec2(static_cast<int>(xpos), static_cast<int>(ypos)) };
		windowSettings->EventCallback(mouseMoveEvent);
	});
#pragma endregion

#pragma region MOUSE_CLICK
	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
		if (action != GLFW_PRESS)
			return;
		auto windowSettings { static_cast<ScreenWindowSettings*>(glfwGetWindowUserPointer(window)) };
		MouseClickEventType clickType {};
		switch (button) {
		case GLFW_MOUSE_BUTTON_MIDDLE:
			clickType = MouseClickEventType::Middle;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			clickType = MouseClickEventType::Left;
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			clickType = MouseClickEventType::Right;
			break;
		}
		MouseClickEvent mouseClickEvent { clickType };
		windowSettings->EventCallback(mouseClickEvent);
	});
#pragma endregion

#pragma region MOUSE_SCROLL
	glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xScroll, double yScroll) {
		auto windowSettings { static_cast<ScreenWindowSettings*>(glfwGetWindowUserPointer(window)) };
		MouseScrollEvent mouseScrollEvent { yScroll > 0 };
		windowSettings->EventCallback(mouseScrollEvent);
	});
#pragma endregion

#pragma region WINDOW_FOCUS
	glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused) {
		auto windowSettings { static_cast<ScreenWindowSettings*>(glfwGetWindowUserPointer(window)) };
		bool focusWon { focused == GLFW_TRUE };
		WindowFocusChangeEvent windowFocusEvent { focusWon };
		windowSettings->EventCallback(windowFocusEvent);
	});
#pragma endregion

#pragma region WINDOW_RESIZE

	glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
		auto windowSettings { static_cast<ScreenWindowSettings*>(glfwGetWindowUserPointer(window)) };

		windowSettings->Height = height;
		windowSettings->Width = width;

		GLCall(glViewport(0, 0, width, height));

		WindowResizeEvent windowResizeEvent { glm::vec2(width, height) };
		windowSettings->EventCallback(windowResizeEvent);
	});

#pragma endregion

#pragma region KEY_PRESS
	glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto windowSettings { static_cast<ScreenWindowSettings*>(glfwGetWindowUserPointer(window)) };
		KeyPressType pressType {};
		switch (action) {
		case GLFW_PRESS:
			pressType = KeyPressType::KeyPressStart;
		case GLFW_RELEASE:
			pressType = KeyPressType::KeyPressStop;
		case GLFW_REPEAT:
			pressType = KeyPressType::KeyPressRepeat;
		}
		KeyPressEvent keyPress { key, pressType };
		windowSettings->EventCallback(keyPress);
	});
#pragma endregion
}

#pragma region SIMPLE_OPERATIONS

bool ScreenWindow::ShouldClose() const {
	return glfwWindowShouldClose(m_Window);
}

void ScreenWindow::OnUpdate() const {
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void ScreenWindow::SetHeight(int height) {
	m_ScreenWindowSettings.Height = height;
}

void ScreenWindow::SetWidth(int width) {
	m_ScreenWindowSettings.Width = width;
}

float ScreenWindow::GetAspectResolution() const {
	return static_cast<float>(m_ScreenWindowSettings.Width) / m_ScreenWindowSettings.Height;
}

int ScreenWindow::GetHeight() const {
	return m_ScreenWindowSettings.Height;
}

int ScreenWindow::GetWidth() const {
	return m_ScreenWindowSettings.Width;
}

GLFWwindow* ScreenWindow::GetNativeWindow() const {
	return m_Window;
}

void ScreenWindow::SetEventCallback(const EventCallbackFn& fn) {
	m_ScreenWindowSettings.EventCallback = fn;
}

#pragma endregion
}
