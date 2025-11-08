
#pragma once
#include "origo/scene/ModelRenderer.h"
#include "origo/scene/SingleComponentSystem.h"

namespace Origo {
class ModelRendererSystem : public SingleComponentSystem<ModelRenderer> {
public:
	void ForEach(const Scene& scene, const std::vector<ModelRenderer*>&) override;
};
}
