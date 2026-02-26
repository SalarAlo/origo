#pragma once

#include "origo/serialization/ISerializer.h"

namespace Origo {

enum class TextureSourceType {
	File = 0,
	Raw,
	Svg,
	Embedded
};

struct TextureInitialisationData {
	TextureInitialisationData() = default;
	TextureInitialisationData(int w, int h, int c, std::vector<unsigned char>&& pixelData, bool generateMipmaps)
	    : Width(w)
	    , Height(h)
	    , Channels(c)
	    , PixelData(std::move(pixelData))
	    , GenerateMipmaps(generateMipmaps) { }

	int Width {};
	int Height {};
	int Channels { 4 };
	std::vector<unsigned char> PixelData {};
	bool GenerateMipmaps { true };
};

class TextureSource {
public:
	virtual ~TextureSource() = default;
	virtual void serialize(ISerializer& backend) const;
	static Scope<TextureSource> deserialize(ISerializer& backend);
	virtual TextureSourceType get_type() const = 0;
	virtual TextureInitialisationData get_initialisation_data() const = 0;

protected:
	virtual void serialize_body(ISerializer& backend) const = 0;
};

class TextureSourceFile : public TextureSource {
public:
	TextureSourceFile(std::string_view path, bool flip = true)
	    : Path(path)
	    , Flip(flip) { }

	void serialize_body(ISerializer& backend) const override;
	TextureSourceType get_type() const override { return TextureSourceType::File; }
	TextureInitialisationData get_initialisation_data() const override;

	std::string Path;
	bool Flip;
};

class TextureSourceRaw : public TextureSource {
public:
	TextureSourceRaw(int w, int h, int c, std::vector<unsigned char>&& d, bool generateMipmaps = true)
	    : InitialisationData(w, h, c, std::move(d), generateMipmaps) { }

	void serialize_body(ISerializer& backend) const override;
	TextureSourceType get_type() const override { return TextureSourceType::Raw; }
	TextureInitialisationData get_initialisation_data() const override;

	TextureInitialisationData InitialisationData {};
};

class TextureSourceSVG : public TextureSource {
public:
	TextureSourceSVG(std::string_view path, int w, int h)
	    : Width(w)
	    , Height(h)
	    , Path(path) {
	}

	void serialize_body(ISerializer& backend) const override;
	TextureSourceType get_type() const override { return TextureSourceType::Svg; }
	TextureInitialisationData get_initialisation_data() const override;

	std::string Path;
	int Width;
	int Height;
};

class TextureSourceEmbedded : public TextureSource {
public:
	TextureSourceEmbedded(
	    std::vector<unsigned char>&& compressedData,
	    bool generateMipmaps = true)
	    : m_compressed_data(std::move(compressedData))
	    , m_generate_mipmaps(generateMipmaps) { }

	TextureSourceEmbedded(
	    int w,
	    int h,
	    int channels,
	    std::vector<unsigned char>&& pixels,
	    bool generateMipmaps = true)
	    : m_raw_data(w, h, channels, std::move(pixels), generateMipmaps)
	    , m_generate_mipmaps(generateMipmaps) { }

	TextureSourceType get_type() const override {
		return TextureSourceType::Embedded;
	}

	TextureInitialisationData get_initialisation_data() const override;

protected:
	void serialize_body(ISerializer& backend) const override;

private:
	std::vector<unsigned char> m_compressed_data {};
	TextureInitialisationData m_raw_data {};

	bool m_generate_mipmaps = true;
};

}
