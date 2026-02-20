#pragma once

#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/PrimitiveShapeCache.h"

#include "origo/components/Transform.h"

#include "origo/components/particle_system/BoxEmissionShape.h"
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
