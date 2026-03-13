#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include <miniaudio.h>

#include "origo/utils/Singleton.h"

namespace Origo {

class AudioEngine : public Singleton<AudioEngine> {
public:
	using PlaybackId = uint64_t;

	AudioEngine();
	~AudioEngine();

	void play_sound(const std::filesystem::path& path);
	std::optional<PlaybackId> create_sound(const std::filesystem::path& path, float volume, bool looping);
	bool has_sound(PlaybackId id) const;
	void resume();
	void pause();
	void set_sound_volume(PlaybackId id, float volume);
	void set_sound_looping(PlaybackId id, bool looping);
	bool is_sound_playing(PlaybackId id) const;
	bool is_sound_at_end(PlaybackId id) const;
	void start_sound(PlaybackId id);
	void stop_sound(PlaybackId id);
	void destroy_sound(PlaybackId id);
	void destroy_all_sounds();

private:
	struct SoundInstance {
		ma_sound Sound {};
		std::string Path {};
	};

	std::unordered_map<PlaybackId, std::shared_ptr<SoundInstance>> m_sounds {};
	PlaybackId m_next_playback_id { 1 };
	ma_engine m_miniaudio_engine;
};

}
