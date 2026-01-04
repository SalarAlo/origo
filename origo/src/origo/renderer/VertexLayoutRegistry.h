#pragma once

#include "origo/renderer/VertexLayout.h"

namespace Origo::VertexLayoutRegistry {

int Register(VertexLayout& layout);
VertexLayout* Get(int id);

}
