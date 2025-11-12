#include "panels/SceneViewport.h"

#include "imgui.h"

namespace OrigoEditor {

void SceneViewport::OnImGuiRender() {
	static float m_Sensitivity { 0.3f };

	ImVec2 size = ImGui::GetContentRegionAvail();
	if (size.x > 0 && size.y > 0 && (m_FrameBuffer.GetWidth() != (int)size.x || m_FrameBuffer.GetHeight() != (int)size.y))
		m_FrameBuffer.Resize((int)size.x, (int)size.y);

	ImGui::Image((ImTextureID)(uintptr_t)m_FrameBuffer.GetColorAttachment(0), size, ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_None)) {
		if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
			if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
				ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
				ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);

				m_Camera.Rotate(delta.x * m_Sensitivity, -delta.y * m_Sensitivity);
			}
		}
	}
}

}
