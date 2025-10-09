#include "origo/ScreenWindow.h"

#include <cassert>
#include <stdexcept>

namespace Origo {
bool ScreenWindow::s_SingleInstanceCreated { false };

void ScreenWindow::InitGlfw() {
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
}

void ScreenWindow::InitGlad() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		throw std::runtime_error("Failed to initialize OpenGL");
	}
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 1);
}

ScreenWindow::ScreenWindow(const ScreenWindowSettings& screenWindowConfig)
    : m_ScreenWindowSettings(screenWindowConfig) {
	if (s_SingleInstanceCreated)
		throw std::runtime_error("There can only be one instance at a time");

	InitGlfw();
	m_Window = glfwCreateWindow(m_ScreenWindowSettings.Width, m_ScreenWindowSettings.Height, m_ScreenWindowSettings.Title.c_str(), nullptr, nullptr);

	glfwMakeContextCurrent(m_Window);

	InitGlad();

	glViewport(0, 0, screenWindowConfig.Width, screenWindowConfig.Height);
	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	s_SingleInstanceCreated = true;
}

bool ScreenWindow::ShouldClose() const {
	return glfwWindowShouldClose(m_Window);
}

void ScreenWindow::SwapBuffers() const {
	glfwSwapBuffers(m_Window);
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
}
