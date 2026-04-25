#include "origo/core/ScreenWindow.h"

#include "origo/events/Event.h"
#include "origo/events/EventTypes.h"
#include "origo/events/KeyEvent.h"
#include "origo/events/MouseEvent.h"
#include "origo/events/WindowEvent.h"

#include "origo/input/Input.h"

#include "origo/renderer/GlDebug.h"

static void APIENTRY my_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam) {
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		return;

	std::cerr << "\n[OpenGL DEBUG MESSAGE]\n";
	std::cerr << "Message: " << message << "\n";
	std::cerr << "Source: " << source << "\n";
	std::cerr << "Type: " << type << "\n";
	std::cerr << "Severity: " << severity << "\n\n";

	if (severity == GL_DEBUG_SEVERITY_HIGH) {
		std::abort();
	}
}

namespace Origo {
bool ScreenWindow::s_single_instance_created { false };

ScreenWindow::~ScreenWindow() {
	if (m_window) {
		glfwDestroyWindow(m_window);
		m_window = nullptr;
	}

	if (s_single_instance_created) {
		glfwTerminate();
		s_single_instance_created = false;
	}
}

void ScreenWindow::init_glfw() {
	if (!glfwInit()) {
		ORG_CORE_ERROR("[ScreenWindow] Failed to initialize GLFW");
		throw std::runtime_error("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHintString(GLFW_WAYLAND_APP_ID, "origo");

	glfwSwapInterval(1);

	glfwWindowHint(GLFW_SAMPLES, 4);
}

void ScreenWindow::init_glad() {
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

	GLCall(glEnable(GL_STENCIL_TEST));
	GLCall(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
	GLCall(glStencilFunc(GL_ALWAYS, 1, 0xFF));
	GLCall(glStencilMask(0xFF));

	GLCall(glPointSize(3.0f));

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(my_callback, nullptr);

	GLCall(glEnable(GL_MULTISAMPLE));
	GLCall(glClearColor(.04, .04, .067, 1));
}

ScreenWindow::ScreenWindow(const ScreenWindowSettings& screenWindowConfig)
    : m_screen_window_settings(screenWindowConfig) {
	if (s_single_instance_created) {
		ORG_CORE_ERROR("[ScreenWindow] There can only be one instance at a time");
		throw std::runtime_error("There can only be one instance at a time");
	}

	init_glfw();
	m_window = glfwCreateWindow(m_screen_window_settings.Width, m_screen_window_settings.Height, m_screen_window_settings.Title.c_str(), nullptr, nullptr);

	glfwMakeContextCurrent(m_window);
	glfwSetWindowUserPointer(m_window, &m_screen_window_settings);

	init_glad();

	Input::set_context(this);
	Input::set_cursor_mode(Input::CursorMode::Locked);

	glfwSwapInterval(1);

	init_callback();

	GLCall(glViewport(0, 0, screenWindowConfig.Width, screenWindowConfig.Height));

	s_single_instance_created = true;
}

void ScreenWindow::init_callback() {
#pragma region MOUSE_MOVE
	glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
		auto window_settings { static_cast<ScreenWindowSettings*>(glfwGetWindowUserPointer(window)) };
		MouseMoveEvent mouse_move_event { Vec2(static_cast<int>(xpos), static_cast<int>(ypos)) };
		window_settings->EventCallback(mouse_move_event);
	});
#pragma endregion

#pragma region MOUSE_CLICK
	glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
		if (action != GLFW_PRESS)
			return;
		auto window_settings { static_cast<ScreenWindowSettings*>(glfwGetWindowUserPointer(window)) };
		MouseClickEventType click_type {};
		switch (button) {
		case GLFW_MOUSE_BUTTON_MIDDLE:
			click_type = MouseClickEventType::Middle;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			click_type = MouseClickEventType::Left;
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			click_type = MouseClickEventType::Right;
			break;
		}
		MouseClickEvent mouse_click_event { click_type };
		window_settings->EventCallback(mouse_click_event);
	});
#pragma endregion

#pragma region MOUSE_SCROLL
	glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xScroll, double yScroll) {
		auto window_settings { static_cast<ScreenWindowSettings*>(glfwGetWindowUserPointer(window)) };
		MouseScrollEvent mouse_scroll_event { yScroll > 0 };
		window_settings->EventCallback(mouse_scroll_event);
	});
#pragma endregion

#pragma region WINDOW_FOCUS
	glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused) {
		auto window_settings { static_cast<ScreenWindowSettings*>(glfwGetWindowUserPointer(window)) };
		bool focus_won { focused == GLFW_TRUE };
		WindowFocusChangeEvent window_focus_event { focus_won };
		window_settings->EventCallback(window_focus_event);
	});
#pragma endregion

#pragma region WINDOW_RESIZE

	glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
		auto window_settings { static_cast<ScreenWindowSettings*>(glfwGetWindowUserPointer(window)) };

		window_settings->Height = height;
		window_settings->Width = width;

		WindowResizeEvent window_resize_event { Vec2(width, height) };
		window_settings->EventCallback(window_resize_event);
	});

#pragma endregion

#pragma region KEY_PRESS
	glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto window_settings { static_cast<ScreenWindowSettings*>(glfwGetWindowUserPointer(window)) };
		KeyPressType press_type {};
		switch (action) {
		case GLFW_PRESS:
			press_type = KeyPressType::KeyPressStart;
			break;
		case GLFW_RELEASE:
			press_type = KeyPressType::KeyPressStop;
			break;
		case GLFW_REPEAT:
			press_type = KeyPressType::KeyPressRepeat;
			break;
		}
		KeyPressEvent key_press { key, press_type };
		window_settings->EventCallback(key_press);
	});
#pragma endregion
}

#pragma region SIMPLE_OPERATIONS

bool ScreenWindow::should_close() const {
	return glfwWindowShouldClose(m_window);
}

void ScreenWindow::on_update() const {
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void ScreenWindow::set_height(int height) {
	m_screen_window_settings.Height = height;
}

void ScreenWindow::set_width(int width) {
	m_screen_window_settings.Width = width;
}

float ScreenWindow::get_aspect_resolution() const {
	return static_cast<float>(m_screen_window_settings.Width) / m_screen_window_settings.Height;
}

int ScreenWindow::get_height() const {
	return m_screen_window_settings.Height;
}

int ScreenWindow::get_width() const {
	return m_screen_window_settings.Width;
}

GLFWwindow* ScreenWindow::get_native_window() const {
	return m_window;
}

void ScreenWindow::set_event_callback(const EventCallbackFn& fn) {
	m_screen_window_settings.EventCallback = fn;
}

#pragma endregion
}
