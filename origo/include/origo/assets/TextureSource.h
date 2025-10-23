#pragma once

#include "nlohmann/json_fwd.hpp"
namespace Origo {

enum class TextureSourceType {
	File,
	Embedded
};

class TextureSource {
public:
	virtual nlohmann::json Serialize() const;
	static Ref<TextureSource> Deserialize(const nlohmann::json&);
	virtual TextureSourceType GetType() const = 0;
};

class TextureSourceFile : public TextureSource {
public:
	TextureSourceFile(std::string_view path)
	    : m_Path(path) { }

	std::string GetPath() const { return m_Path; }

	nlohmann::json Serialize() const override;
	TextureSourceType GetType() const override { return TextureSourceType::File; }

private:
	std::string m_Path;
};

class TextureSourceEmbedded : public TextureSource {
public:
	nlohmann::json Serialize() const override;
	TextureSourceType GetType() const override { return TextureSourceType::Embedded; };

private:
};

}
