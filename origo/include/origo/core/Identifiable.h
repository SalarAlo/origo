#pragma once

#include "origo/core/UUID.h"

namespace Origo {
class Identifiable {
public:
	UUID GetId() const { return m_Id; };

protected:
	UUID m_Id {};
};
}
