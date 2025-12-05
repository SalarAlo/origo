#include "origo/assets/TextureSource.h"
#include "origo/core/Logger.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

void TextureSource::Serialize(ISerializer& backend) const {
	backend.BeginObject("source");
	backend.Write("type", magic_enum::enum_name(GetType()));
	SerializeBody(backend);
	backend.EndObject();
}

void TextureSourceFile::SerializeBody(ISerializer& backend) const {
	backend.Write("path", m_Path);
}

Scope<TextureSource> TextureSource::Deserialize(ISerializer& backend) {
	backend.BeginObject("source");

	std::string typeStr {};
	backend.TryRead("type", typeStr);
	auto optionalType { magic_enum::enum_cast<TextureSourceType>(typeStr) };

	if (!optionalType.has_value()) {
		backend.EndObject();
		ORG_INFO("Texture Source type {} is unidentifiable", typeStr);
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
	default: {
		backend.EndObject();
		ORG_ERROR("TextureSource::Deserialize: Unsupported type {}", typeStr);
		return nullptr;
	}
	}
}

}
