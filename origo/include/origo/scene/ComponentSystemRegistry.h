#pragma once

#include "origo/scene/ISystem.h"
namespace Origo {

class ComponentSystemRegistry {
public:
	static ComponentSystemRegistry& GetInstance() {
		static ComponentSystemRegistry s_Instance;
		return s_Instance;
	}

	void Register(std::unique_ptr<ISystem> system) {
		m_Systems.push_back(std::move(system));
	}

	void RunAll(Scene& scene) {
		for (auto& system : m_Systems)
			system->Run(scene);
	}

	ComponentSystemRegistry(const ComponentSystemRegistry&) = delete;
	ComponentSystemRegistry(ComponentSystemRegistry&&) = delete;
	ComponentSystemRegistry& operator=(const ComponentSystemRegistry&) = delete;
	ComponentSystemRegistry& operator=(ComponentSystemRegistry&&) = delete;

private:
	ComponentSystemRegistry() = default;

private:
	std::vector<std::unique_ptr<ISystem>> m_Systems {};
};

}

#define REGISTER_SYSTEM(TYPE)                                                                                                 \
	namespace Origo {                                                                                                     \
		struct TYPE##AutoRegister {                                                                                   \
			TYPE##AutoRegister() { ComponentSystemRegistry ::GetInstance().Register(std ::make_unique<TYPE>()); } \
		};                                                                                                            \
		static TYPE##AutoRegister s_##TYPE##AutoRegister;                                                             \
	}
