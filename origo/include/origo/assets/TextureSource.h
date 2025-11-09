#pragma once

#include "origo/serialization/ISerializer.h"
namespace Origo {

enum class TextureSourceType {
	Empty = 0,
	File,
	Embedded,
};

class TextureSource {
public:
	virtual ~TextureSource() = default;
	virtual void Serialize(ISerializer& backend) const;
	static TextureSource* Deserialize(ISerializer& backend);
	virtual TextureSourceType GetType() const = 0;
};

class TextureSourceFile : public TextureSource {
public:
	TextureSourceFile(std::string_view path)
	    : m_Path(path) { }

	std::string GetPath() const { return m_Path; }

	void Serialize(ISerializer& backend) const override;
	TextureSourceType GetType() const override { return TextureSourceType::File; }

private:
	std::string m_Path;
};

class TextureSourceEmbedded : public TextureSource {
public:
	void Serialize(ISerializer& backend) const override;
	TextureSourceType GetType() const override { return TextureSourceType::Embedded; };

private:
};

class TextureSourceEmpty : public TextureSource {
public:
	TextureSourceEmpty(int width, int height)
	    : m_Width(width)
	    , m_Height(height) { }
	void Serialize(ISerializer& backend) const override;
	TextureSourceType GetType() const override { return TextureSourceType::Empty; };

	int GetHeight() const { return m_Height; }
	int GetWidth() const { return m_Width; }

private:
	int m_Width;
	int m_Height;
};

}
