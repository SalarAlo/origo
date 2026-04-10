#pragma once

#include <optional>

#include "origo/assets/AssetManager.h"

#include "origo/audio/AudioEngine.h"

#include "origo/components/Component.h"

namespace Origo {

struct SoundEmitterComponent : public Component {
	OptionalAssetHandle AudioHandle {};

	float Volume { 1.0f };
	float Pitch { 1.0f };
	float StartDelaySeconds { 0.0f };

	bool IsEnabled { true };
	bool AutoPlay { true };
	bool IsLooping {};
	bool IsMuted {};

	bool IsSpatial {};
	float FalloffStartDistance {};
	float FalloffDuration {};

	bool FirstPlayDone {};
	std::optional<AudioEngine::PlaybackId> PlaybackId {};
	OptionalAssetHandle LastAudioHandle {};

	float DelayRemainingSeconds { 0.0f };
	bool PlayRequested {};
	bool StopRequested {};
	bool RestartRequested {};
	bool WasEnabledLastFrame {};

	void request_play() {
		PlayRequested = true;
		StopRequested = false;
	}

	void request_stop() {
		StopRequested = true;
		PlayRequested = false;
		RestartRequested = false;
	}

	void request_restart() {
		RestartRequested = true;
		PlayRequested = false;
		StopRequested = false;
	}

	void reset_runtime_state() {
		FirstPlayDone = false;
		PlaybackId.reset();
		LastAudioHandle.reset();
		DelayRemainingSeconds = 0.0f;
		PlayRequested = false;
		StopRequested = false;
		RestartRequested = false;
		WasEnabledLastFrame = false;
	}

	std::string get_component_name() const { return "Sound Emitter"; }
};

}
