#include "origo/imgui/ImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace Origo {

void ImGuiLayer::OnAttach(const ScreenWindow& window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Enable docking + viewports
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // useful
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // main feature
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // multiple windows

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();

	// Make pop-out windows consistent with main window
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	GLFWwindow* native = window.GetNativeWindow();
	ImGui_ImplGlfw_InitForOpenGL(native, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void ImGuiLayer::OnDetach() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiLayer::Begin() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiLayer::End() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Multi-viewport support: must update and render platform windows
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		GLFWwindow* backupContext = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backupContext);
	}
}

} // namespace Origo
