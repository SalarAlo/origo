#include "origo/renderer/PostProcessor.h"

#include "origo/assets/Shader.h"

#include "origo/core/PathContext.h"

namespace Origo {
namespace {

FrameBufferSpec create_post_buffer_spec(int width, int height) {
	FrameBufferSpec spec;
	spec.Width = width;
	spec.Height = height;
	spec.Attachments = {
		{ AttachmentType::Color, GL_RGBA16F, GL_RGBA, GL_FLOAT },
	};
	return spec;
}

float clamp_positive(float value, float fallback) {
	return value > 0.0f ? value : fallback;
}

void bind_texture(GLuint texture, int slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texture);
}

}

void PostProcessor::apply(const FrameBuffer& source, FrameBuffer& target, const PostProcessSettings& settings, float time_seconds) {
	ensure_resources(source.get_width(), source.get_height());
	target.resize(source.get_width(), source.get_height());

	const GLuint bloom_texture = run_bloom(source, settings);

	target.bind();
	glViewport(0, 0, target.get_width(), target.get_height());
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	m_composite_shader->use_program();

	bind_texture(source.get_color_attachment(0), 0);
	m_composite_shader->set_uniform("u_scene_color", 0);

	bind_texture(bloom_texture, 1);
	m_composite_shader->set_uniform("u_bloom_texture", 1);

	m_composite_shader->set_uniform("u_post_enabled", settings.Enabled);
	m_composite_shader->set_uniform("u_has_bloom", bloom_texture != 0 && settings.Enabled && settings.Bloom.Enabled);
	m_composite_shader->set_uniform("u_bloom_intensity", settings.Bloom.Intensity);

	m_composite_shader->set_uniform("u_tonemap_enabled", settings.ToneMapping.Enabled && settings.Enabled);
	m_composite_shader->set_uniform("u_tonemap_operator", static_cast<int>(settings.ToneMapping.Operator));
	m_composite_shader->set_uniform("u_exposure", settings.ToneMapping.Exposure);
	m_composite_shader->set_uniform("u_gamma", clamp_positive(settings.ToneMapping.Gamma, 2.2f));

	m_composite_shader->set_uniform("u_color_adjustments_enabled", settings.ColorAdjustments.Enabled && settings.Enabled);
	m_composite_shader->set_uniform("u_contrast", settings.ColorAdjustments.Contrast);
	m_composite_shader->set_uniform("u_saturation", settings.ColorAdjustments.Saturation);
	m_composite_shader->set_uniform("u_hue_shift", settings.ColorAdjustments.HueShift);
	m_composite_shader->set_uniform("u_temperature", settings.ColorAdjustments.Temperature);
	m_composite_shader->set_uniform("u_tint", settings.ColorAdjustments.Tint);

	m_composite_shader->set_uniform("u_vignette_enabled", settings.Vignette.Enabled && settings.Enabled);
	m_composite_shader->set_uniform("u_vignette_intensity", settings.Vignette.Intensity);
	m_composite_shader->set_uniform("u_vignette_smoothness", settings.Vignette.Smoothness);
	m_composite_shader->set_uniform("u_vignette_roundness", settings.Vignette.Roundness);
	m_composite_shader->set_uniform("u_vignette_color", settings.Vignette.Color);

	m_composite_shader->set_uniform("u_chromatic_enabled", settings.ChromaticAberration.Enabled && settings.Enabled);
	m_composite_shader->set_uniform("u_chromatic_intensity", settings.ChromaticAberration.Intensity);

	m_composite_shader->set_uniform("u_film_grain_enabled", settings.FilmGrain.Enabled && settings.Enabled);
	m_composite_shader->set_uniform("u_film_grain_intensity", settings.FilmGrain.Intensity);
	m_composite_shader->set_uniform("u_film_grain_response", settings.FilmGrain.Response);

	m_composite_shader->set_uniform("u_time", time_seconds);
	m_composite_shader->set_uniform("u_inv_resolution", Vec2(1.0f / source.get_width(), 1.0f / source.get_height()));

	render_fullscreen_triangle();
	target.unbind();
}

void PostProcessor::ensure_resources(int width, int height) {
	if (m_fullscreen_vao == 0)
		glGenVertexArrays(1, &m_fullscreen_vao);

	const auto shader_root = PathContextService::get_instance().editor().fallback_root() / "shaders";
	ensure_shader(m_composite_shader, shader_root / "post_process.glsl");
	ensure_shader(m_bloom_prefilter_shader, shader_root / "post_bloom_prefilter.glsl");
	ensure_shader(m_bloom_blur_shader, shader_root / "post_bloom_blur.glsl");

	if (!m_bloom_ping) {
		m_bloom_ping = make_scope<FrameBuffer>(create_post_buffer_spec(width, height));
	} else {
		m_bloom_ping->resize(width, height);
	}

	if (!m_bloom_pong) {
		m_bloom_pong = make_scope<FrameBuffer>(create_post_buffer_spec(width, height));
	} else {
		m_bloom_pong->resize(width, height);
	}
}

void PostProcessor::ensure_shader(Scope<Shader>& shader, const std::filesystem::path& path) {
	if (shader)
		return;

	shader = make_scope<Shader>();
	shader->set_source(make_scope<ShaderSourceFile>(path));
	shader->resolve();
}

void PostProcessor::render_fullscreen_triangle() const {
	glBindVertexArray(m_fullscreen_vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

GLuint PostProcessor::run_bloom(const FrameBuffer& source, const PostProcessSettings& settings) {
	if (!settings.Enabled || !settings.Bloom.Enabled || settings.Bloom.Intensity <= 0.0f)
		return 0;

	m_bloom_ping->bind();
	glViewport(0, 0, source.get_width(), source.get_height());
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	m_bloom_prefilter_shader->use_program();
	bind_texture(source.get_color_attachment(0), 0);
	m_bloom_prefilter_shader->set_uniform("u_scene_color", 0);
	m_bloom_prefilter_shader->set_uniform("u_threshold", settings.Bloom.Threshold);
	m_bloom_prefilter_shader->set_uniform("u_knee", glm::max(settings.Bloom.Knee, 0.0001f));
	render_fullscreen_triangle();

	const int blur_passes = std::clamp(settings.Bloom.BlurPasses, 1, 12);
	bool horizontal = true;
	GLuint input_texture = m_bloom_ping->get_color_attachment(0);

	for (int i = 0; i < blur_passes * 2; ++i) {
		FrameBuffer& target = horizontal ? *m_bloom_pong : *m_bloom_ping;
		target.bind();
		glViewport(0, 0, target.get_width(), target.get_height());

		m_bloom_blur_shader->use_program();
		bind_texture(input_texture, 0);
		m_bloom_blur_shader->set_uniform("u_image", 0);
		m_bloom_blur_shader->set_uniform(
		    "u_direction",
		    horizontal ? Vec2(1.0f / source.get_width(), 0.0f) : Vec2(0.0f, 1.0f / source.get_height()));
		render_fullscreen_triangle();

		input_texture = target.get_color_attachment(0);
		horizontal = !horizontal;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return input_texture;
}

}
