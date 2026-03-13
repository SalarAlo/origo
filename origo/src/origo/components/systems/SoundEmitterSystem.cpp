#include <algorithm>

#include "origo/components/systems/SoundEmitterSystem.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/Audio.h"

#include "origo/audio/AudioEngine.h"

#include "origo/components/SoundEmitter.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {

void SoundEmitterSystem::update(Origo::Scene* scene, float dt) {
	auto& audio_engine = AudioEngine::get_instance();

	scene->view<SoundEmitter>([&](RID emitter_rid, SoundEmitter& sound_emitter) {
		if (!sound_emitter.AudioHandle) {
			if (sound_emitter.PlaybackId) {
				audio_engine.destroy_sound(*sound_emitter.PlaybackId);
				sound_emitter.PlaybackId.reset();
			}
			sound_emitter.LastAudioHandle.reset();
			sound_emitter.FirstPlayDone = false;
			return;
		}

		if (sound_emitter.LastAudioHandle != sound_emitter.AudioHandle) {
			if (sound_emitter.PlaybackId) {
				audio_engine.destroy_sound(*sound_emitter.PlaybackId);
				sound_emitter.PlaybackId.reset();
			}
			sound_emitter.FirstPlayDone = false;
			sound_emitter.LastAudioHandle = sound_emitter.AudioHandle;
		}

		AssetHandle handle { *sound_emitter.AudioHandle };
		Audio* audio = AssetManager::get_instance().get_asset<Audio>(handle);
		if (!audio) {
			ORG_WARN("SoundEmitterSystem: audio handle is invalid or no longer points to an Audio asset");
			return;
		}

		audio->resolve();
		if (!audio->AudioPath.has_value()) {
			ORG_ERROR("No audio path provided");
			return;
		}

		const float volume = std::max(0.0f, sound_emitter.Volume);

		if (sound_emitter.PlaybackId) {
			if (!audio_engine.has_sound(*sound_emitter.PlaybackId)) {
				sound_emitter.PlaybackId.reset();
			} else {
				audio_engine.set_sound_volume(*sound_emitter.PlaybackId, volume);
				audio_engine.set_sound_looping(*sound_emitter.PlaybackId, sound_emitter.IsLooping);

				if (!sound_emitter.IsLooping && audio_engine.is_sound_at_end(*sound_emitter.PlaybackId)) {
					audio_engine.destroy_sound(*sound_emitter.PlaybackId);
					sound_emitter.PlaybackId.reset();
				} else {
					if (sound_emitter.IsLooping && !audio_engine.is_sound_playing(*sound_emitter.PlaybackId))
						audio_engine.start_sound(*sound_emitter.PlaybackId);
					return;
				}
			}
		}

		if (sound_emitter.FirstPlayDone && !sound_emitter.IsLooping)
			return;

		auto playback_id = audio_engine.create_sound(*audio->AudioPath, volume, sound_emitter.IsLooping);
		if (!playback_id.has_value())
			return;

		audio_engine.start_sound(*playback_id);
		sound_emitter.PlaybackId = *playback_id;
		sound_emitter.FirstPlayDone = true;
	});
}

REGISTER_UPDATE_SYSTEM(
    Origo::GamePhase::UpdateGame,
    Origo::SoundEmitterSystem);
}
