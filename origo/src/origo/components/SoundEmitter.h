#pragma once

#include <optional>

#include "origo/assets/AssetManager.h"
#include "origo/audio/AudioEngine.h"

#include "origo/components/Component.h"

namespace Origo {

struct SoundEmitter : public Component {
	OptionalAssetHandle AudioHandle {};
	float Volume { 1.0f };
	bool IsLooping {};

	bool FirstPlayDone {};
	std::optional<AudioEngine::PlaybackId> PlaybackId {};
	OptionalAssetHandle LastAudioHandle {};

	void reset_runtime_state() {
		FirstPlayDone = false;
		PlaybackId.reset();
		LastAudioHandle.reset();
	}

	std::string get_component_name() const { return "Sound Emitter"; }
};

}
