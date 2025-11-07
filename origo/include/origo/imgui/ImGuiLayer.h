
#pragma once

namespace Origo {
class ImGuiLayer {
public:
	ImGuiLayer() = default;
	~ImGuiLayer() = default;

	void OnAttach(GLFWwindow* window);
	void OnDetach();
	void Begin();
	void End();
};
}
