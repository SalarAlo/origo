#pragma once

#include "ui/IInspectorDrawable.h"

namespace OrigoEditor {

template <typename T>
class InspectorDrawable : public IInspectorDrawable {
public:
	using DrawFn = std::function<void(T&)>;
	InspectorDrawable(std::string_view name, DrawFn drawer)
	    : m_Name(name.data())
	    , m_DrawFn(drawer) {
	}

	void Draw(void* drawablePtr) override {
		m_DrawFn(*reinterpret_cast<T*>(drawablePtr));
	}

	const char* GetName() const override {
		return m_Name;
	}

private:
	const char* m_Name;
	DrawFn m_DrawFn;
};

}
