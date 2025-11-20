#pragma once

#include "origo/core/RID.h"

namespace Origo {
class Identifiable {
public:
	RID GetId() const { return m_Id; };

protected:
	RID m_Id { RID::New() };
};
}
