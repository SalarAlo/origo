#pragma once

#include "origo/serialization/ISerializer.h"
namespace Origo {

enum class TextureSourceType {
	File = 0,
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
	    : m_Path(path) { }

	std::string GetPath() const { return m_Path; }

	void SerializeBody(ISerializer& backend) const override;
	TextureSourceType GetType() const override { return TextureSourceType::File; }

private:
	std::string m_Path;
};

}
