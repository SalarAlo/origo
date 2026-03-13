#include "origo/assets/Audio.h"

#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetManager.h"

#include "origo/audio/AudioEngine.h"

namespace Origo {

Audio::Audio(const std::optional<std::filesystem::path>& audio_path)
    : AudioPath(audio_path) {
	ORG_INFO("Audio asset constructed");
}

void Audio::play_sound() {
	resolve();

	if (!AudioPath.has_value()) {
		ORG_ERROR("No audio path provided");
		return;
	}

	AudioEngine::get_instance().play_sound(*AudioPath);
}

void Audio::resolve() {
	if (AudioPath.has_value())
		return;

	auto uuid = AssetManager::get_instance().get_uuid(this);
	if (!uuid.has_value())
		return;

	const auto& meta = AssetDatabase::get_instance().get_metadata(*uuid);
	if (!meta.SourcePath.has_value())
		return;

	AudioPath = *meta.SourcePath;
}

}
