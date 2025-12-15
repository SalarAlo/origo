#include "panels/SceneViewport.h"
#include "imgui.h"

namespace OrigoEditor {

void SceneViewport::OnImGuiRender() {
	constexpr float sensitivity = 0.3f;
	constexpr float maxDelta = 50.0f;
	constexpr float smoothAlpha = 0.2f;

	ImVec2 size = ImGui::GetContentRegionAvail();
	int w = (int)size.x;
	int h = (int)size.y;

	if (!m_RenderTarget || !m_ResolveTarget || w <= 0 || h <= 0) {
		ImGui::Dummy(size);
		return;
	}

	static int lastW = 0, lastH = 0;
	bool resized = false;

	if (w != lastW || h != lastH) {
		m_RenderTarget->Resize(w, h);
		m_ResolveTarget->Resize(w, h);
		lastW = w;
		lastH = h;
		resized = true;
	}

	ImGui::Image(
	    (ImTextureID)(uintptr_t)m_ResolveTarget->GetColorAttachment(0),
	    size,
	    ImVec2(0, 1),
	    ImVec2(1, 0));

	const bool hovered = ImGui::IsItemHovered();
	ImGuiIO& io = ImGui::GetIO();

	static bool dragging = false;
	static ImVec2 smoothDelta { 0.0f, 0.0f };

	if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		dragging = true;

	if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
		dragging = false;

	if (!dragging || resized)
		return;

	ImVec2 delta = io.MouseDelta;

	delta.x = std::clamp(delta.x, -maxDelta, maxDelta);
	delta.y = std::clamp(delta.y, -maxDelta, maxDelta);

	smoothDelta.x = smoothDelta.x * (1.0f - smoothAlpha) + delta.x * smoothAlpha;
	smoothDelta.y = smoothDelta.y * (1.0f - smoothAlpha) + delta.y * smoothAlpha;

	m_Camera.Rotate(
	    smoothDelta.x * sensitivity,
	    -smoothDelta.y * sensitivity);
}

}
