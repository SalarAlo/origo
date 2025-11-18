#pragma once

#include "IComponentDrawer.h"
#include "origo/scene/Scene.h"

namespace OrigoEditor {

template <Origo::ComponentType T>
class ComponentDrawer : public IComponentDrawer {
public:
	using DrawFn = std::function<void(T&)>;
	ComponentDrawer(std::string_view name, DrawFn drawFn)
	    : m_Name(name.data())
	    , m_DrawFn(std::move(drawFn)) { }

	const char* GetName() const override { return m_Name; }

	void Draw(void* componentPtr) override {
		m_DrawFn(*reinterpret_cast<T*>(componentPtr));
	}

private:
	const char* m_Name;
	DrawFn m_DrawFn;
};
}
