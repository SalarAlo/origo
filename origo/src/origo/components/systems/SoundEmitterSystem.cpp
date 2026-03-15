#include <algorithm>

#include "origo/components/systems/SoundEmitterSystem.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/Audio.h"

#include "origo/audio/AudioEngine.h"

#include "origo/components/CameraComponent.h"
#include "origo/components/SoundEmitter.h"
#include "origo/components/Transform.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {

void SoundEmitterSystem::update(Origo::Scene* scene, float dt) {
	auto& audio_engine = AudioEngine::get_instance();

	Vec3 camera_position {};
	Vec3 camera_forward { 0.0f, 0.0f, -1.0f };
	Vec3 camera_up { 0.0f, 1.0f, 0.0f };

	scene->view<CameraComponent, TransformComponent>([&](RID, CameraComponent& cc, TransformComponent& tr) {
		if (cc.IsPrimary) {
			camera_position = tr.get_position();
			camera_forward = tr.get_forward();
			camera_up = tr.get_up();
		}
	});

	audio_engine.set_listener_transform(camera_position, camera_forward, camera_up);

	scene->view<SoundEmitter>([&](RID entity, SoundEmitter& sound_emitter) {
		auto destroy_playback = [&]() {
			if (!sound_emitter.PlaybackId)
				return;

			audio_engine.destroy_sound(*sound_emitter.PlaybackId);
			sound_emitter.PlaybackId.reset();
		};

		const bool is_active = sound_emitter.IsEnabled && sound_emitter.AudioHandle.has_value();
		const bool reenabled = sound_emitter.IsEnabled && !sound_emitter.WasEnabledLastFrame;

		if (!is_active) {
			destroy_playback();
			sound_emitter.FirstPlayDone = false;
			sound_emitter.DelayRemainingSeconds = 0.0f;
			if (!sound_emitter.AudioHandle)
				sound_emitter.LastAudioHandle.reset();
			sound_emitter.WasEnabledLastFrame = sound_emitter.IsEnabled;
			return;
		}

		const bool audio_changed = sound_emitter.LastAudioHandle != sound_emitter.AudioHandle;
		if (audio_changed || sound_emitter.RestartRequested || reenabled) {
			destroy_playback();
			sound_emitter.FirstPlayDone = false;
			sound_emitter.DelayRemainingSeconds = std::max(0.0f, sound_emitter.StartDelaySeconds);
			sound_emitter.LastAudioHandle = sound_emitter.AudioHandle;
		}

		if (sound_emitter.StopRequested) {
			destroy_playback();
			sound_emitter.FirstPlayDone = false;
			sound_emitter.DelayRemainingSeconds = std::max(0.0f, sound_emitter.StartDelaySeconds);
		}

		const bool start_requested = sound_emitter.PlayRequested || sound_emitter.RestartRequested;
		if (start_requested && !sound_emitter.RestartRequested && !sound_emitter.PlaybackId && sound_emitter.DelayRemainingSeconds <= 0.0f)
			sound_emitter.DelayRemainingSeconds = std::max(0.0f, sound_emitter.StartDelaySeconds);

		sound_emitter.PlayRequested = false;
		sound_emitter.StopRequested = false;
		sound_emitter.RestartRequested = false;

		AssetHandle handle { *sound_emitter.AudioHandle };
		Audio* audio = AssetManager::get_instance().get_asset<Audio>(handle);
		if (!audio) {
			ORG_WARN("SoundEmitterSystem: audio handle is invalid or no longer points to an Audio asset");
			sound_emitter.WasEnabledLastFrame = sound_emitter.IsEnabled;
			return;
		}

		audio->resolve();
		if (!audio->AudioPath.has_value()) {
			ORG_ERROR("No audio path provided");
			sound_emitter.WasEnabledLastFrame = sound_emitter.IsEnabled;
			return;
		}

		const float volume = sound_emitter.IsMuted ? 0.0f : std::max(0.0f, sound_emitter.Volume);
		const float pitch = std::max(0.01f, sound_emitter.Pitch);
		const TransformComponent* emitter_transform = scene->get_native_component<TransformComponent>(entity);
		const Vec3 emitter_position = emitter_transform ? emitter_transform->get_position() : camera_position;

		if (sound_emitter.PlaybackId) {
			if (!audio_engine.has_sound(*sound_emitter.PlaybackId)) {
				sound_emitter.PlaybackId.reset();
			} else {
				audio_engine.set_sound_volume(*sound_emitter.PlaybackId, volume);
				audio_engine.set_sound_pitch(*sound_emitter.PlaybackId, pitch);
				audio_engine.set_sound_looping(*sound_emitter.PlaybackId, sound_emitter.IsLooping);
				audio_engine.set_sound_spatial(
				    *sound_emitter.PlaybackId,
				    sound_emitter.IsSpatial,
				    emitter_position,
				    sound_emitter.FalloffStartDistance,
				    sound_emitter.FalloffDuration);

				if (!sound_emitter.IsLooping && audio_engine.is_sound_at_end(*sound_emitter.PlaybackId)) {
					audio_engine.destroy_sound(*sound_emitter.PlaybackId);
					sound_emitter.PlaybackId.reset();
				} else {
					if (sound_emitter.IsLooping && !audio_engine.is_sound_playing(*sound_emitter.PlaybackId))
						audio_engine.start_sound(*sound_emitter.PlaybackId);
					sound_emitter.WasEnabledLastFrame = sound_emitter.IsEnabled;
					return;
				}
			}
		}

		const bool should_autoplay = sound_emitter.AutoPlay && (!sound_emitter.FirstPlayDone || sound_emitter.IsLooping);
		if (!start_requested && !should_autoplay) {
			sound_emitter.WasEnabledLastFrame = sound_emitter.IsEnabled;
			return;
		}

		if (sound_emitter.FirstPlayDone && !sound_emitter.IsLooping && !start_requested) {
			sound_emitter.WasEnabledLastFrame = sound_emitter.IsEnabled;
			return;
		}

		if (sound_emitter.DelayRemainingSeconds > 0.0f) {
			sound_emitter.DelayRemainingSeconds = std::max(0.0f, sound_emitter.DelayRemainingSeconds - dt);
			if (sound_emitter.DelayRemainingSeconds > 0.0f) {
				sound_emitter.WasEnabledLastFrame = sound_emitter.IsEnabled;
				return;
			}
		}

		auto playback_id = audio_engine.create_sound(*audio->AudioPath, volume, sound_emitter.IsLooping, pitch);
		if (!playback_id.has_value()) {
			sound_emitter.WasEnabledLastFrame = sound_emitter.IsEnabled;
			return;
		}

		audio_engine.start_sound(*playback_id);
		audio_engine.set_sound_spatial(
		    *playback_id,
		    sound_emitter.IsSpatial,
		    emitter_position,
		    sound_emitter.FalloffStartDistance,
		    sound_emitter.FalloffDuration);
		sound_emitter.PlaybackId = *playback_id;
		sound_emitter.FirstPlayDone = true;
		sound_emitter.WasEnabledLastFrame = sound_emitter.IsEnabled;
	});
}

REGISTER_UPDATE_SYSTEM(
    Origo::GamePhase::UpdateGame,
    Origo::SoundEmitterSystem);
}
