
#pragma once

namespace Origo {
class ImGuiLayer {
public:
	ImGuiLayer() = default;
	~ImGuiLayer() = default;

	void on_attach(GLFWwindow* window);
	void on_detach();
	void begin();
	void end();
};
}
