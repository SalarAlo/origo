#include "origo/components/systems/ParticleSystemComponentSystem.h"

#include "glm/ext/matrix_transform.hpp"

#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/PrimitiveShapeCache.h"

#include "origo/components/ParticleSystemComponent.h"
#include "origo/components/Transform.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {

static inline float RandomRange(float min, float max) {
	return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

void ParticleSystemComponentSystem::Render(Origo::Scene* scene, RenderContext& rCtx) {
	scene->View<ParticleSystemComponent, TransformComponent>([&](RID, ParticleSystemComponent& ps, TransformComponent& tr) {
		for (auto i { 0uz }; i < ps.SpawnSpeed; i++) {

			glm::mat4 model = tr.GetModelMatrix();

			glm::vec3 randomOffset = glm::vec3(
			    RandomRange(-1.0f, 1.0f),
			    RandomRange(-1.0f, 1.0f),
			    RandomRange(-1.0f, 1.0f));

			glm::mat4 offset = glm::translate(glm::mat4(1.0f), randomOffset);
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), ps.StartSize);
			offset *= scale;

			glm::mat4 finalModel = model * offset;

			rCtx.SubmitMesh(
			    PrimitiveShapeCache::GetInstance().GetCubeMesh(),
			    DefaultAssetCache::GetInstance().GetMaterial(),
			    finalModel);
		}
	});
}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderGame,
    Origo::ParticleSystemComponentSystem);

}
