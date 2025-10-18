#include "origo/scene/Light.h"

namespace Origo {

Light::Light(LightType type, Entity& e)
    : Component(e)
    , m_Type(type) {
}

}
