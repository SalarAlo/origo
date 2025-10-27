#include "origo/assets/TextureSource.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

void TextureSource::Serialize(ISerializer& backend) const {
	backend.BeginObject("source");
	backend.Write("type", magic_enum::enum_name(GetType()));
}

void TextureSourceFile::Serialize(ISerializer& backend) const {
	TextureSource::Serialize(backend);
	backend.Write("path", m_Path);
	backend.EndObject();
}

void TextureSourceEmbedded::Serialize(ISerializer& backend) const {
	TextureSource::Serialize(backend);
	// todo
	backend.EndObject();
}

Ref<TextureSource> TextureSource::Deserialize(ISerializer& backend) {
	// todo
	return nullptr;
}

}
