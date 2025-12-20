#include "origo/assets/Texture.h"

#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvg.h"
#include "nanosvgrast.h"
#include "origo/assets/TextureSource.h"
#include "origo/assets/Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_SIMD
#include "stb_image.h"

#include "origo/renderer/GlDebug.h"
#include "origo/core/Logger.h"

namespace Origo {

Texture::Texture(TextureType type)
    : m_TextureType(type)
    , m_TextureId(0) {
}

void Texture::SetSource(Scope<TextureSource> src) {
	m_Source = std::move(src);
}

void Texture::LoadCPU() {
	if (!m_Source) {
		ORG_ERROR("Texture::LoadCPU: No TextureSource assigned");
		return;
	}

	if (auto* file = dynamic_cast<TextureSourceFile*>(m_Source.get())) {
		const std::string& path = file->Path;
		stbi_set_flip_vertically_on_load(true);

		int width = 0, height = 0, channels = 0;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (!data) {
			ORG_ERROR("Texture::LoadCPU: failed to load image {}", path);
			return;
		}

		InitTexture(width, height, channels, data);

		stbi_image_free(data);
		return;
	}

	if (auto* mem = dynamic_cast<TextureSourceRaw*>(m_Source.get())) {
		InitTexture(mem->Width, mem->Height, mem->Channels, mem->Data.data());
		return;
	}

	if (auto* svgTex = dynamic_cast<TextureSourceSVG*>(m_Source.get())) {
		ORG_INFO("Loading SVG Texture");

		NSVGimage* svg = nsvgParseFromFile(svgTex->Path.c_str(), "px", 96.0f);
		if (!svg)
			return;

		NSVGrasterizer* rast = nsvgCreateRasterizer();
		if (!rast) {
			nsvgDelete(svg);
			return;
		}

		int w = svgTex->Width;
		int h = svgTex->Height;
		float scale = w / (svg->width > 0.0f ? svg->width : (float)w);

		std::vector<unsigned char> pixels(w * h * 4);
		nsvgRasterize(rast, svg, 0.0f, 0.0f, scale, pixels.data(), w, h, w * 4);

		nsvgDeleteRasterizer(rast);
		nsvgDelete(svg);

		InitTexture(w, h, 4, pixels.data());
		return;
	}

	ORG_ERROR("Texture::LoadCPU: Unsupported TextureSource type");
}

void Texture::LoadCpuIfTextureNotExistent() {
	if (m_TextureId != 0)
		return;

	LoadCPU();
}

void Texture::InitTexture(int width, int height, int channels, unsigned char* data) {
	GLCall(glGenTextures(1, &m_TextureId));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureId));

	GLenum format = (channels == 4 ? GL_RGBA : GL_RGB);

	GLCall(glTexImage2D(
	    GL_TEXTURE_2D,
	    0,
	    format,
	    width,
	    height,
	    0,
	    format,
	    GL_UNSIGNED_BYTE,
	    data));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture::Bind(AssetHandle shaderId) const {
	const_cast<Texture*>(this)->LoadCpuIfTextureNotExistent();

	if (m_TextureId == 0) {
		ORG_ERROR("Texture::Bind: GPU texture not initialized");
		return;
	}

	auto shader = AssetManagerFast::GetInstance().Get<Shader>(shaderId);
	int slot = static_cast<int>(m_TextureType);

	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureId));

	std::string uniformName = "u_Texture_";
	uniformName += std::string(magic_enum::enum_name(m_TextureType));

	shader->SetUniform(uniformName, slot);
}

}
