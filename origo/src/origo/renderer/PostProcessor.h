#pragma once

#include "origo/assets/Shader.h"
#include "origo/renderer/FrameBuffer.h"
#include "origo/renderer/PostProcessSettings.h"

namespace Origo {

class PostProcessor {
public:
	void apply(const FrameBuffer& source, FrameBuffer& target, const PostProcessSettings& settings, float time_seconds);

private:
	void ensure_resources(int width, int height);
	void ensure_shader(Scope<Shader>& shader, const std::filesystem::path& path);
	void render_fullscreen_triangle() const;
	GLuint run_bloom(const FrameBuffer& source, const PostProcessSettings& settings);

private:
	GLuint m_fullscreen_vao {};

	Scope<Shader> m_composite_shader {};
	Scope<Shader> m_bloom_prefilter_shader {};
	Scope<Shader> m_bloom_blur_shader {};

	Scope<FrameBuffer> m_bloom_ping {};
	Scope<FrameBuffer> m_bloom_pong {};
};

}
