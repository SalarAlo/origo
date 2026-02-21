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

// TODO: Ensure depth buffer always wins some different kind of pass
struct RenderEmissionShapeDebug {
	RenderEmissionShapeDebug(RenderContext& rCtx, const Vec3& position)
	    : RenderCtx(rCtx)
	    , Position(position) {
	}

	RenderContext& RenderCtx;
	const Vec3& Position;

	void operator()(const PointEmissionShape&) const {
		constexpr float POINT_SIZE = 0.05f;

		TransformComponent t;
		t.SetPosition(Position);
		t.SetScale(Vec3 { POINT_SIZE });

		RenderCtx.SubmitMesh(
		    PrimitiveShapeCache::GetInstance().GetSphereMesh(),
		    DefaultAssetCache::GetInstance().GetParticleEmissionDebugMaterial(),
		    t.GetModelMatrix(),
		    RenderPass::Geometry,
		    GL_TRIANGLES);
	}

	void operator()(const ConeEmissionShape& c) const {
		TransformComponent t;

		Vec3 dir = c.Direction;
		if (length(dir) < 1e-6f)
			dir = Vec3 { 0, 1, 0 };
		Vec3 axis = normalize(dir);

		float angleRad = glm::radians(c.Angle);
		float radius = c.Length * tanf(angleRad);

		glm::quat rot = glm::rotation(
		    glm::vec3(0, 1, 0),
		    -glm::normalize(glm::vec3(axis.x, axis.y, axis.z)));

		glm::vec3 eulerDeg = glm::degrees(glm::eulerAngles(rot));
		t.SetRotation(Vec3 { eulerDeg.x, eulerDeg.y, eulerDeg.z });

		t.SetScale(Vec3 { 2.0f * radius, c.Length, 2.0f * radius });

		glm::vec3 tipLocal(0.0f, 0.5f * c.Length, 0.0f);
		glm::vec3 tipOffsetWorld = rot * tipLocal;

		Vec3 tipPos = Position + c.Position;
		t.SetPosition(tipPos - Vec3 { tipOffsetWorld.x, tipOffsetWorld.y, tipOffsetWorld.z });

		RenderCtx.SubmitMesh(
		    PrimitiveShapeCache::GetInstance().GetConeMesh(),
		    DefaultAssetCache::GetInstance().GetParticleEmissionDebugMaterial(),
		    t.GetModelMatrix(),
		    RenderPass::Geometry,
		    GL_LINES);
	}

	void operator()(const SphereEmissionShape& s) const {
		TransformComponent t;
		t.SetPosition(Position);
		t.SetScale(Vec3 { s.Radius * 2.0f });

		RenderCtx.SubmitMesh(
		    PrimitiveShapeCache::GetInstance().GetSphereMesh(),
		    DefaultAssetCache::GetInstance().GetParticleEmissionDebugMaterial(),
		    t.GetModelMatrix(),
		    RenderPass::Geometry,
		    GL_LINES);
	}

	void operator()(const BoxEmissionShape& b) const {
		TransformComponent t;
		t.SetPosition(Position);
		t.SetScale(Vec3 { b.Size });

		RenderCtx.SubmitMesh(
		    PrimitiveShapeCache::GetInstance().GetCubeMesh(),
		    DefaultAssetCache::GetInstance().GetParticleEmissionDebugMaterial(),
		    t.GetModelMatrix(),
		    RenderPass::Geometry,
		    GL_LINE_LOOP);
	}
};
}
