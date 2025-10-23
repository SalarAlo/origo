#include "origo/assets/TextureSource.h"
#include "nlohmann/json.hpp"

namespace Origo {

nlohmann::json TextureSource::Serialize() const {
	return { { "type", magic_enum::enum_name(GetType()) } };
}

nlohmann::json TextureSourceFile::Serialize() const {
	nlohmann::json j = TextureSource::Serialize();
	j["path"] = m_Path;
	return j;
}

nlohmann::json TextureSourceEmbedded::Serialize() const {
	nlohmann::json j = TextureSource::Serialize();
	return j;
}

Ref<TextureSource> TextureSource::Deserialize(const nlohmann::json& j) {
	auto typeStr = j.at("type").get<std::string>();
	auto type = magic_enum::enum_cast<TextureSourceType>(typeStr).value();

	switch (type) {
	case TextureSourceType::File:
		return TextureSourceFile::Deserialize(j);
	case TextureSourceType::Embedded:
		return TextureSourceEmbedded::Deserialize(j);
	}
	return nullptr;
}

}
