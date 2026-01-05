#include "origo/assets/TextureSource.h"
#include "origo/core/Logger.h"
#include "origo/serialization/ISerializer.h"

#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvg.h"
#include "nanosvgrast.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_SIMD
#include "stb_image.h"

namespace Origo {

void TextureSource::Serialize(ISerializer& backend) const {
	backend.BeginObject("source");
	backend.Write("type", magic_enum::enum_name(GetType()));
	SerializeBody(backend);
	backend.EndObject();
}

void TextureSourceRaw::SerializeBody(ISerializer& backend) const {
	backend.Write("width", InitialisationData.Width);
	backend.Write("height", InitialisationData.Height);
	backend.Write("channels", InitialisationData.Channels);
	backend.Write("generate_mipmaps", InitialisationData.GenerateMipmaps);

	backend.BeginArray("Data");
	for (const auto& entry : InitialisationData.PixelData) {
		backend.Write(entry);
	}
	backend.EndArray();
}

void TextureSourceSVG::SerializeBody(ISerializer& backend) const {
	backend.Write("width", Width);
	backend.Write("height", Height);
	backend.Write("path", Path);
}

void TextureSourceFile::SerializeBody(ISerializer& backend) const {
	backend.Write("path", Path);
}

TextureInitialisationData TextureSourceFile::GetInitialisationData() const {
	stbi_set_flip_vertically_on_load(Flip);

	TextureInitialisationData initialisationData {};
	const auto pixels = stbi_load(Path.c_str(), &initialisationData.Width, &initialisationData.Height, &initialisationData.Channels, 4);
	initialisationData.Channels = 4;

	if (!pixels) {
		ORG_CORE_ERROR("Texture::LoadCPU: failed to load image {}", Path);
		return {};
	}
	const size_t byteCount {
		static_cast<size_t>(initialisationData.Height) * static_cast<size_t>(initialisationData.Width) * static_cast<size_t>(initialisationData.Channels)
	};
	initialisationData.PixelData = std::vector<unsigned char>(pixels, pixels + byteCount);

	stbi_image_free(pixels);

	return initialisationData;
}

TextureInitialisationData TextureSourceRaw::GetInitialisationData() const {
	return InitialisationData;
}

TextureInitialisationData TextureSourceSVG::GetInitialisationData() const {
	NSVGimage* svg = nsvgParseFromFile(Path.c_str(), "px", 96.0f);
	if (!svg) {
		ORG_CORE_WARN("TextureSourceSVG::GetInitialisationData: Failed to parse from file path {}", Path);
		return {};
	}

	NSVGrasterizer* rast = nsvgCreateRasterizer();
	if (!rast) {
		ORG_CORE_WARN("TextureSourceSVG::GetInitialisationData: Failed to create rasterizer");
		nsvgDelete(svg);
		return {};
	}

	constexpr int SVG_OVERSAMPLE = 4;

	int w = Width * SVG_OVERSAMPLE;
	int h = Height * SVG_OVERSAMPLE;

	float scale = w / svg->width;

	std::vector<unsigned char> pixels(w * h * 4);
	nsvgRasterize(rast, svg, 0.0f, 0.0f, scale, pixels.data(), w, h, w * 4);

	nsvgDeleteRasterizer(rast);
	nsvgDelete(svg);

	return { w, h, 4, std::move(pixels), false };
}

void TextureSourceEmbedded::SerializeBody(ISerializer& backend) const {
	// Intentionally empty.
	// Embedded textures are reconstructed during model import.
}

TextureInitialisationData TextureSourceEmbedded::GetInitialisationData() const {
	if (!m_RawData.PixelData.empty()) {
		return m_RawData;
	}

	int w, h, channels;

	stbi_uc* pixels = stbi_load_from_memory(
	    m_CompressedData.data(),
	    static_cast<int>(m_CompressedData.size()),
	    &w,
	    &h,
	    &channels,
	    4);

	if (!pixels) {
		ORG_ERROR("TextureSourceEmbedded: Failed to decode embedded texture");
		return {};
	}

	std::vector<unsigned char> data(
	    pixels,
	    pixels + (w * h * 4));

	stbi_image_free(pixels);

	return TextureInitialisationData(
	    w,
	    h,
	    4,
	    std::move(data),
	    m_GenerateMipmaps);
}

Scope<TextureSource> TextureSource::Deserialize(ISerializer& backend) {
	backend.BeginObject("source");

	std::string typeStr {};
	backend.TryRead("type", typeStr);
	auto optionalType { magic_enum::enum_cast<TextureSourceType>(typeStr) };

	if (!optionalType.has_value()) {
		backend.EndObject();
		ORG_CORE_WARN("Texture Source type {} is unidentifiable", typeStr);
		return nullptr;
	}

	auto type = optionalType.value();

	switch (type) {
	case TextureSourceType::File: {
		std::string path {};
		backend.TryRead("path", path);
		backend.EndObject();

		return MakeScope<TextureSourceFile>(path);
	}

	case TextureSourceType::Embedded:
		ORG_CORE_ERROR(
		    "TextureSourceEmbedded cannot be deserialized independently. "
		    "Reimport the parent model.");
		return nullptr;
	default: {
		backend.EndObject();
		ORG_ERROR("TextureSource::Deserialize: Unsupported type {}", typeStr);
		return nullptr;
	}
	}
}

}
