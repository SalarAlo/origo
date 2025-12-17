#pragma once

#include "origo/Camera.h"
#include "origo/renderer/FrameBuffer.h"
#include "origo/scene/Transform.h"
#include "origo/renderer/RenderCommand.h"

namespace Origo {

class RenderContext {
public:
	RenderContext(FrameBuffer* target = nullptr)
	    : m_Target(target) {
	}

	void SetTarget(FrameBuffer* target) { m_Target = target; }
	void SetResolveTarget(FrameBuffer* resolveTarget) { m_ResolveTarget = resolveTarget; }

	FrameBuffer* GetTarget() const { return m_Target; }
	FrameBuffer* GetResolveTarget() const { return m_ResolveTarget; }
	FrameBuffer* GetFinalTarget() const { return m_ResolveTarget ? m_ResolveTarget : m_Target; }

	void BeginFrame();
	void EndFrame();

	void Submit(const AssetHandle& mesh, const AssetHandle& material, Transform* transform, RenderPass pass = RenderPass::Geometry);
	void Flush(Camera* camera);

	void SetDrawMethod(GLenum drawMethod) { m_DrawMethod = drawMethod; }

private:
	void Clear();
	void BindFB();
	void ExecutePass(RenderPass pass, Camera* camera);
	void ConfigureState(RenderPass pass);
	void Resolve();

private:
	FrameBuffer* m_Target {};
	FrameBuffer* m_ResolveTarget {};
	std::vector<RenderCommand> m_DrawQueue {};
	GLenum m_DrawMethod { GL_TRIANGLES };
};

}
