#pragma once

#include "InspectorDrawable.h"
#include "imgui.h"
#include "ui/IInspectorDrawable.h"
#include <typeindex>
#include <unordered_map>

namespace OrigoEditor {

class InspectorDrawRegistry {
public:
	template <typename T>
	static void Register(const char* name, InspectorDrawable<T>::DrawFn fn) {
		m_Drawers[typeid(T)] = Origo::MakeScope<InspectorDrawable<T>>(name, std::move(fn));
	}

	static void Draw(void* drawablePtr, std::type_index type) {
		auto it = m_Drawers.find(type);

		if (it == m_Drawers.end()) {
			std::string header = "Unknown Inspector Drawable##" + std::to_string(reinterpret_cast<uintptr_t>(drawablePtr));
			if (ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				ImGui::TextDisabled("(No inspector drawer registered)");
			return;
		}

		if (ImGui::CollapsingHeader(it->second->GetName(), ImGuiTreeNodeFlags_DefaultOpen))
			it->second->Draw(drawablePtr);
	}

private:
	inline static std::unordered_map<std::type_index, Origo::Scope<IInspectorDrawable>> m_Drawers;
};

}
