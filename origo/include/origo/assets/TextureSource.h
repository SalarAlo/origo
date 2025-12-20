#pragma once

#include "origo/serialization/ISerializer.h"
namespace Origo {

enum class TextureSourceType {
	File = 0,
	Raw,
	Svg
};

class TextureSource {
public:
	virtual ~TextureSource() = default;
	virtual void Serialize(ISerializer& backend) const;
	static Scope<TextureSource> Deserialize(ISerializer& backend);
	virtual TextureSourceType GetType() const = 0;

protected:
	virtual void SerializeBody(ISerializer& backend) const = 0;
};

class TextureSourceFile : public TextureSource {
public:
	TextureSourceFile(std::string_view path)
	    : Path(path) { }

	void SerializeBody(ISerializer& backend) const override;
	TextureSourceType GetType() const override { return TextureSourceType::File; }

	std::string Path;
};

class TextureSourceRaw : public TextureSource {
public:
	TextureSourceRaw(int w, int h, int c, std::vector<unsigned char>&& d)
	    : Width(w)
	    , Height(h)
	    , Channels(c)
	    , Data(std::move(d)) { }

	void SerializeBody(ISerializer& backend) const override;
	TextureSourceType GetType() const override { return TextureSourceType::Raw; }

	int Width;
	int Height;
	int Channels;
	std::vector<unsigned char> Data;
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

	std::string Path;
	int Width;
	int Height;
};

}
