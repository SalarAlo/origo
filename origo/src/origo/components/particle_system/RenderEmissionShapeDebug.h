#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/PrimitiveShapeCache.h"

#include "origo/components/Transform.h"

#include "origo/components/particle_system/BoxEmissionShape.h"
#include "origo/components/particle_system/ConeEmissionShape.h"
#include "origo/components/particle_system/PointEmissionShape.h"
#include "origo/components/particle_system/SphereEmissionShape.h"

#include "origo/renderer/RenderContext.h"

namespace Origo {

struct RenderEmissionShapeDebug {
	RenderEmissionShapeDebug(RenderContext& rCtx, TransformComponent& emitter)
	    : RenderCtx(rCtx)
	    , Emitter(emitter) { }

	RenderContext& RenderCtx;
	TransformComponent& Emitter;

	glm::mat4 GetEmitterMatrix() const {
		return Emitter.GetModelMatrix();
	}

	void operator()(const PointEmissionShape&) const {
		constexpr float POINT_SIZE = 0.05f;

		glm::mat4 model = GetEmitterMatrix() * glm::scale(glm::mat4(1.0f), glm::vec3(POINT_SIZE));

		RenderCtx.SubmitMesh(
		    PrimitiveShapeCache::GetInstance().GetSphereMesh(),
		    DefaultAssetCache::GetInstance().GetParticleEmissionDebugMaterial(),
		    model,
		    RenderPass::Geometry,
		    GL_TRIANGLES);
	}

	void operator()(const ConeEmissionShape& c) const {
		Vec3 dir = c.Direction;
		if (length(dir) < 1e-6f)
			dir = Vec3 { 0, 1, 0 };

		float angleRad = glm::radians(c.Angle);
		float radius = c.Length * tanf(angleRad);

		glm::quat rot = glm::rotation(
		    glm::vec3(0, 1, 0),
		    -glm::normalize(glm::vec3(dir.x, dir.y, dir.z)));

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(c.Position)) * glm::toMat4(rot) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f * c.Length, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f * radius, c.Length, 2.0f * radius));

		RenderCtx.SubmitMesh(
		    PrimitiveShapeCache::GetInstance().GetConeMesh(),
		    DefaultAssetCache::GetInstance().GetParticleEmissionDebugMaterial(),
		    model,
		    RenderPass::Geometry,
		    GL_LINES);
	}

	void operator()(const SphereEmissionShape& s) const {
		glm::mat4 local = glm::scale(glm::mat4(1.0f), glm::vec3(s.Radius * 2.0f));

		glm::mat4 model = GetEmitterMatrix() * local;

		RenderCtx.SubmitMesh(
		    PrimitiveShapeCache::GetInstance().GetSphereMesh(),
		    DefaultAssetCache::GetInstance().GetParticleEmissionDebugMaterial(),
		    model,
		    RenderPass::Geometry,
		    GL_LINES);
	}

	void operator()(const BoxEmissionShape& b) const {
		glm::mat4 local = glm::scale(glm::mat4(1.0f), glm::vec3(b.Size));

		glm::mat4 model = GetEmitterMatrix() * local;

		RenderCtx.SubmitMesh(
		    PrimitiveShapeCache::GetInstance().GetCubeMesh(),
		    DefaultAssetCache::GetInstance().GetParticleEmissionDebugMaterial(),
		    model,
		    RenderPass::Geometry,
		    GL_LINE_LOOP);
	}
};

}
