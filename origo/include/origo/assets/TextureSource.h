#pragma once

#include "origo/serialization/ISerializer.h"
namespace Origo {

enum class TextureSourceType {
	File,
	Embedded
};

class TextureSource {
public:
	virtual void Serialize(ISerializer& backend) const;
	static Ref<TextureSource> Deserialize(ISerializer& backend);
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

}
