#pragma once

#include "origo/renderer/VertexLayout.h"

namespace Origo::VertexLayoutRegistry {

int register_layout(VertexLayout& layout);
VertexLayout* get(int id);

}
