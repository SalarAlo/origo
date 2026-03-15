#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include <miniaudio.h>

#include "origo/core/Typedefs.h"
#include "origo/utils/Singleton.h"

namespace Origo {

class AudioEngine : public Singleton<AudioEngine> {
public:
	using PlaybackId = uint64_t;

	AudioEngine();
	~AudioEngine();

	void play_sound(const std::filesystem::path& path);
	std::optional<PlaybackId> create_sound(const std::filesystem::path& path, float volume, bool looping, float pitch = 1.0f);
	bool has_sound(PlaybackId id) const;
	void resume();
	void pause();
	void set_sound_volume(PlaybackId id, float volume);
	void set_sound_pitch(PlaybackId id, float pitch);
	void set_sound_looping(PlaybackId id, bool looping);
	void set_listener_transform(const Vec3& position, const Vec3& direction, const Vec3& up);
	void set_sound_spatial(PlaybackId id, bool is_spatial, const Vec3& position, float falloff_start_distance, float falloff_duration);
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
