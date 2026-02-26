#pragma once

#include "ui/IInspectorDrawable.h"

namespace OrigoEditor {

template <typename T>
class InspectorDrawable : public IInspectorDrawable {
public:
	using DrawFn = std::function<void(T&)>;

	InspectorDrawable(std::string_view name, DrawFn drawer)
	    : m_name(name.data())
	    , m_draw_fn(drawer) {
	}

	void draw(void* drawablePtr) override {
		m_draw_fn(*reinterpret_cast<T*>(drawablePtr));
	}

	const char* get_name() const override {
		return m_name;
	}

private:
	const char* m_name;
	DrawFn m_draw_fn;
};

}
