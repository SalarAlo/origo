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
	    : m_target(target) {
	}

	void set_target(FrameBuffer* target) { m_target = target; }
	void set_resolve_target(FrameBuffer* resolveTarget) { m_resolve_target = resolveTarget; }
	void set_skybox_material(AssetHandle skyboxMaterial);

	void push_point_light(const PointLightData& data);
	void push_directional_light(const DirectionalLightData& data);

	void set_view(const RenderView& view);

	void clear_view() { m_has_view = false; }

	FrameBuffer* get_target() const { return m_target; }
	FrameBuffer* get_resolve_target() const { return m_resolve_target; }
	FrameBuffer* get_final_target() const { return m_resolve_target ? m_resolve_target : m_target; }

	void begin_frame();
	void end_frame();

	void submit_mesh(const AssetHandle& mesh, const AssetHandle& material, const glm::mat4& modelMatrix, RenderPass pass = RenderPass::Geometry, GLenum drawMethod = GL_TRIANGLES);
	void submit_model(const AssetHandle& mesh, const glm::mat4& modelMatrix, RenderPass pass = RenderPass::Geometry, const std::optional<AssetHandle>& optionalMaterial = std::nullopt);

	void flush();

private:
	void clear();
	void bind_fb();
	void execute_pass(RenderPass pass);
	void configure_state(RenderPass pass);

private:
	FrameBuffer* m_target {};
	FrameBuffer* m_resolve_target {};

	std::optional<DirectionalLightData> m_directional_light_data;
	std::vector<PointLightData> m_point_lights;

	OptionalAssetHandle m_skybox_material { std::nullopt };
	GLuint m_skybox_vao {};

	RenderView m_view {};
	bool m_has_view {};

	std::vector<RenderCommand> m_draw_queue {};
};

}
