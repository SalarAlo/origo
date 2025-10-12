#pragma once

#include "origo/renderer/VertexArray.h"
#include "origo/renderer/VertexBuffer.h"

namespace Origo {
class Batch {
private:
	VertexBuffer m_VertexBuffer;
	VertexArray m_VertexArray;
};

}
