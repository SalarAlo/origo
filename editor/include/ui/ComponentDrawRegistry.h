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
	static void Register(const char* name, ComponentDrawer<T>::DrawFn fn) {
		m_Drawers[typeid(T)] = Origo::MakeScope<ComponentDrawer<T>>(name, fn);
	}

	template <typename T>
	static void Draw(T* component) {
		auto it = m_Drawers.find(typeid(T));
		if (it == m_Drawers.end()) {
			if (ImGui::CollapsingHeader("To be continued...", ImGuiTreeNodeFlags_DefaultOpen)) {
			}
			return;
		}

		it->second->Draw(component);
	}

private:
	inline static std::unordered_map<std::type_index, Origo::Scope<IComponentDrawer>> m_Drawers {};
};
}
