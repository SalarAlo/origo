#pragma once

#include <optional>

#include "origo/components/DirectionalLightData.h"
#include "origo/components/PointLightData.h"

#include "origo/renderer/FrameBuffer.h"
#include "origo/renderer/RenderCommand.h"
#include "origo/renderer/RenderView.h"

namespace Origo {

class RenderContext {
public:
	RenderContext(FrameBuffer* target = nullptr)
	    : m_Target(target) {
	}

	void SetTarget(FrameBuffer* target) { m_Target = target; }
	void SetResolveTarget(FrameBuffer* resolveTarget) { m_ResolveTarget = resolveTarget; }
	void SetSkyboxMaterial(AssetHandle skyboxMaterial);

	void PushPointLight(const PointLightData& data);
	void PushDirectionalLight(const DirectionalLightData& data);

	void SetView(const RenderView& view);

	void ClearView() { m_HasView = false; }

	FrameBuffer* GetTarget() const { return m_Target; }
	FrameBuffer* GetResolveTarget() const { return m_ResolveTarget; }
	FrameBuffer* GetFinalTarget() const { return m_ResolveTarget ? m_ResolveTarget : m_Target; }

	void BeginFrame();
	void EndFrame();

	void SubmitMesh(
	    const AssetHandle& mesh,
	    const AssetHandle& material,
	    const glm::mat4& modelMatrix,
	    RenderPass pass = RenderPass::Geometry,
	    GLenum drawMethod = GL_TRIANGLES);
	void SubmitModel(
	    const AssetHandle& mesh,
	    const glm::mat4& modelMatrix,
	    RenderPass pass = RenderPass::Geometry,
	    const std::optional<AssetHandle>& optionalMaterial = std::nullopt);
	void Flush();

private:
	void Clear();
	void BindFB();
	void ExecutePass(RenderPass pass);
	void ConfigureState(RenderPass pass);

private:
	FrameBuffer* m_Target {};
	FrameBuffer* m_ResolveTarget {};

	std::optional<DirectionalLightData> m_DirectionalLightData;
	std::vector<PointLightData> m_PointLights;

	OptionalAssetHandle m_SkyboxMaterial { std::nullopt };
	GLuint m_SkyboxVAO {};

	RenderView m_View {};
	bool m_HasView {};

	std::vector<RenderCommand> m_DrawQueue {};
};

}
