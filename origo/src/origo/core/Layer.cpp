#include "origo/core/Layer.h"
#include <string_view>

namespace Origo {
Layer::Layer(std::string_view name)
    : m_debug_name(name) { }
Layer::~Layer() { }
}
