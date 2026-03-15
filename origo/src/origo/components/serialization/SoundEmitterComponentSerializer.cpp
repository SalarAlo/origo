#include "origo/components/serialization/SoundEmitterComponentSerializer.h"

#include "origo/components/SoundEmitter.h"

namespace Origo {
void SoundEmitterComponentSerializer::serialize(Component* comp, ISerializer& s) const {
	auto sound_emitter { dynamic_cast<SoundEmitter*>(comp) };

	s.write("is_enabled", sound_emitter->IsEnabled);
	s.write("auto_play", sound_emitter->AutoPlay);
	s.write("is_looping", sound_emitter->IsLooping);
	s.write("is_muted", sound_emitter->IsMuted);
	s.write("volume", sound_emitter->Volume);
	s.write("pitch", sound_emitter->Pitch);
	s.write("is_spatial", sound_emitter->IsSpatial);
	s.write("falloff_start_dist", sound_emitter->FalloffStartDistance);
	s.write("falloff_dur", sound_emitter->FalloffDuration);
	s.write("start_delay_seconds", sound_emitter->StartDelaySeconds);
	s.write("audio_id", sound_emitter->AudioHandle);
}

void SoundEmitterComponentSerializer::deserialize(Component* comp, ISerializer& s) {
	auto sound_emitter { dynamic_cast<SoundEmitter*>(comp) };

	s.try_read("is_enabled", sound_emitter->IsEnabled);
	s.try_read("auto_play", sound_emitter->AutoPlay);
	s.try_read("is_looping", sound_emitter->IsLooping);
	s.try_read("is_muted", sound_emitter->IsMuted);
	s.try_read("volume", sound_emitter->Volume);
	s.try_read("pitch", sound_emitter->Pitch);
	s.try_read("start_delay_seconds", sound_emitter->StartDelaySeconds);
	s.try_read("is_spatial", sound_emitter->IsSpatial);
	s.try_read("falloff_start_dist", sound_emitter->FalloffStartDistance);
	s.try_read("falloff_dur", sound_emitter->FalloffDuration);

	if (std::string audio_id; s.try_read("audio_id", audio_id)) {
		auto uuid = UUID::from_string(audio_id);
		sound_emitter->AudioHandle = AssetManager::get_instance().get_handle_by_uuid(uuid);
	}
}

}
