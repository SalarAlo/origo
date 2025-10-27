
#pragma once

#include "origo/core/ScreenWindow.h"

namespace Origo {
class ImGuiLayer {
public:
	ImGuiLayer() = default;
	~ImGuiLayer() = default;

	void OnAttach(const ScreenWindow& window);
	void OnDetach();
	void Begin();
	void End();
};
}
