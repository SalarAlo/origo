#include "origo/assets/TextureSource.h"

#include "origo/core/PathContext.h"

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

void TextureSource::serialize(ISerializer& backend) const {
	backend.begin_object("source");
	backend.write("type", magic_enum::enum_name(get_type()));
	serialize_body(backend);
	backend.end_object();
}

void TextureSourceRaw::serialize_body(ISerializer& backend) const {
	backend.write("width", InitialisationData.Width);
	backend.write("height", InitialisationData.Height);
	backend.write("channels", InitialisationData.Channels);
	backend.write("generate_mipmaps", InitialisationData.GenerateMipmaps);

	backend.begin_array("Data");
	for (const auto& entry : InitialisationData.PixelData) {
		backend.write(entry);
	}
	backend.end_array();
}

void TextureSourceSVG::serialize_body(ISerializer& backend) const {
	backend.write("width", Width);
	backend.write("height", Height);
	backend.write("path", PathContextService::get_instance().serialize_path(Path).string());
}

void TextureSourceFile::serialize_body(ISerializer& backend) const {
	backend.write("path", PathContextService::get_instance().serialize_path(Path).string());
}

TextureInitialisationData TextureSourceFile::get_initialisation_data() const {
	stbi_set_flip_vertically_on_load(Flip);

	TextureInitialisationData initialisation_data {};
	const auto pixels = stbi_load(Path.c_str(), &initialisation_data.Width, &initialisation_data.Height, &initialisation_data.Channels, 4);
	initialisation_data.Channels = 4;

	if (!pixels) {
		ORG_CORE_ERROR("Texture::LoadCPU: failed to load image {}", Path.string());
		return {};
	}
	const size_t byte_count {
		static_cast<size_t>(initialisation_data.Height) * static_cast<size_t>(initialisation_data.Width) * static_cast<size_t>(initialisation_data.Channels)
	};
	initialisation_data.PixelData = std::vector<unsigned char>(pixels, pixels + byte_count);

	stbi_image_free(pixels);

	return initialisation_data;
}

TextureInitialisationData TextureSourceRaw::get_initialisation_data() const {
	return InitialisationData;
}

TextureInitialisationData TextureSourceSVG::get_initialisation_data() const {
	NSVGimage* svg = nsvgParseFromFile(Path.c_str(), "px", 96.0f);
	if (!svg) {
		ORG_CORE_WARN("TextureSourceSVG::get_initialisation_data: Failed to parse from file path {}", Path.string());
		return {};
	}

	NSVGrasterizer* rast = nsvgCreateRasterizer();
	if (!rast) {
		ORG_CORE_WARN("TextureSourceSVG::get_initialisation_data: Failed to create rasterizer");
		nsvgDelete(svg);
		return {};
	}

	constexpr int svg_oversample = 4;

	int w = Width * svg_oversample;
	int h = Height * svg_oversample;

	float scale = w / svg->width;

	std::vector<unsigned char> pixels(w * h * 4);
	nsvgRasterize(rast, svg, 0.0f, 0.0f, scale, pixels.data(), w, h, w * 4);

	nsvgDeleteRasterizer(rast);
	nsvgDelete(svg);

	return { w, h, 4, std::move(pixels), false };
}

void TextureSourceEmbedded::serialize_body(ISerializer& backend) const {
	// Intentionally empty.
	// Embedded textures are reconstructed during model import.
}

TextureInitialisationData TextureSourceEmbedded::get_initialisation_data() const {
	if (!m_raw_data.PixelData.empty()) {
		return m_raw_data;
	}

	int w, h, channels;

	stbi_uc* pixels = stbi_load_from_memory(
	    m_compressed_data.data(),
	    static_cast<int>(m_compressed_data.size()),
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
	    m_generate_mipmaps);
}

Scope<TextureSource> TextureSource::deserialize(ISerializer& backend) {
	backend.begin_object("source");

	std::string type_str {};
	backend.try_read("type", type_str);
	auto optional_type { magic_enum::enum_cast<TextureSourceType>(type_str) };

	if (!optional_type.has_value()) {
		backend.end_object();
		ORG_CORE_WARN("Texture Source type {} is unidentifiable", type_str);
		return nullptr;
	}

	auto type = optional_type.value();

	switch (type) {
	case TextureSourceType::File: {
		std::string path {};
		backend.try_read("path", path);
		backend.end_object();

		return make_scope<TextureSourceFile>(PathContextService::get_instance().resolve_serialized_path(path));
	}

	case TextureSourceType::Embedded:
		ORG_CORE_ERROR(
		    "TextureSourceEmbedded cannot be deserialized independently. "
		    "Reimport the parent model.");
		return nullptr;
	default: {
		backend.end_object();
		ORG_ERROR("TextureSource::Deserialize: Unsupported type {}", type_str);
		return nullptr;
	}
	}
}

}
