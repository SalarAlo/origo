#pragma once

#include "origo/serialization/ISerializer.h"
namespace Origo {

enum class TextureSourceType {
	File = 0,
	Raw,
	Svg
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
	virtual void Serialize(ISerializer& backend) const;
	static Scope<TextureSource> Deserialize(ISerializer& backend);
	virtual TextureSourceType GetType() const = 0;
	virtual TextureInitialisationData GetInitialisationData() const = 0;

protected:
	virtual void SerializeBody(ISerializer& backend) const = 0;
};

class TextureSourceFile : public TextureSource {
public:
	TextureSourceFile(std::string_view path, bool flip = true)
	    : Path(path)
	    , Flip(flip) { }

	void SerializeBody(ISerializer& backend) const override;
	TextureSourceType GetType() const override { return TextureSourceType::File; }
	TextureInitialisationData GetInitialisationData() const override;

	std::string Path;
	bool Flip;
};

class TextureSourceRaw : public TextureSource {
public:
	TextureSourceRaw(int w, int h, int c, std::vector<unsigned char>&& d, bool generateMipmaps = true)
	    : InitialisationData(w, h, c, std::move(d), generateMipmaps) { }

	void SerializeBody(ISerializer& backend) const override;
	TextureSourceType GetType() const override { return TextureSourceType::Raw; }
	TextureInitialisationData GetInitialisationData() const override;

	TextureInitialisationData InitialisationData {};
};

class TextureSourceSVG : public TextureSource {
public:
	TextureSourceSVG(std::string_view path, int w, int h)
	    : Width(w)
	    , Height(h)
	    , Path(path) {
	}

	void SerializeBody(ISerializer& backend) const override;
	TextureSourceType GetType() const override { return TextureSourceType::Svg; }
	TextureInitialisationData GetInitialisationData() const override;

	std::string Path;
	int Width;
	int Height;
};

}
