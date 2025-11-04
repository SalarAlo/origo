#pragma once

namespace OrigoEditor {

class IComponentDrawer {
public:
	virtual ~IComponentDrawer() = default;
	virtual const char* GetName() const = 0;
	virtual void Draw(void* componentPtr) = 0;
};

}
