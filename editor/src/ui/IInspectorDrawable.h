#pragma once

namespace OrigoEditor {

class IInspectorDrawable {
public:
	virtual ~IInspectorDrawable() = default;
	virtual const char* get_name() const = 0;
	virtual void draw(void* componentPtr) = 0;
};

};
