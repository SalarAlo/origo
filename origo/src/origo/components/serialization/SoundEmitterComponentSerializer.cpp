#include "origo/components/serialization/SoundEmitterComponentSerializer.h"

#include "origo/components/SoundEmitter.h"

namespace Origo {
void SoundEmitterComponentSerializer::serialize(Component* comp, ISerializer& s) const {
	auto sound_emitter { dynamic_cast<SoundEmitter*>(comp) };

	s.write("is_looping", sound_emitter->IsLooping);
	s.write("volume", sound_emitter->Volume);
	s.write("audio_id", sound_emitter->AudioHandle);
}

void SoundEmitterComponentSerializer::deserialize(Component* comp, ISerializer& s) {
	auto sound_emitter { dynamic_cast<SoundEmitter*>(comp) };

	s.try_read("is_looping", sound_emitter->IsLooping);
	s.try_read("volume", sound_emitter->Volume);

	if (std::string audio_id; s.try_read("audio_id", audio_id)) {
		auto uuid = UUID::from_string(audio_id);
		sound_emitter->AudioHandle = AssetManager::get_instance().get_handle_by_uuid(uuid);
	}
}

}
