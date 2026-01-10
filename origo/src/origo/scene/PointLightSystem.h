#include "origo/scene/IUpdateSystem.h"
#include "origo/scene/PointLight.h"
#include "origo/scene/Scene.h"
#include "origo/assets/Shader.h"
#include "origo/scene/Transform.h"

namespace Origo {

class PointLightSystem final : public Origo::IUpdateSystem {
public:
	void Update(Origo::Scene* scene, float dt) override;
};
}
