#pragma once

#include "ComponentDrawer.h"
#include "imgui.h"
#include "ui/IComponentDrawer.h"
#include <typeindex>
#include <unordered_map>

namespace OrigoEditor {

class ComponentDrawRegistry {
public:
	template <typename T>
	static void Register(const char* name, typename ComponentDrawer<T>::DrawFn fn) {
		m_Drawers[typeid(T)] = Origo::MakeScope<ComponentDrawer<T>>(name, std::move(fn));
	}

	static void Draw(void* componentPtr, std::type_index type) {
		auto it = m_Drawers.find(type);

		if (it == m_Drawers.end()) {
			std::string header = "Unknown Component##" + std::to_string(reinterpret_cast<uintptr_t>(componentPtr));
			if (ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				ImGui::TextDisabled("(No component drawer registered)");
			return;
		}

		if (ImGui::CollapsingHeader(it->second->GetName(), ImGuiTreeNodeFlags_DefaultOpen))
			it->second->Draw(componentPtr);
	}

private:
	inline static std::unordered_map<std::type_index, Origo::Scope<IComponentDrawer>> m_Drawers;
};

}
