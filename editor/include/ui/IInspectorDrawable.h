#pragma once

namespace OrigoEditor {

class IInspectorDrawable {
public:
	virtual ~IInspectorDrawable() = default;
	virtual const char* GetName() const = 0;
	virtual void Draw(void* componentPtr) = 0;
};

};
