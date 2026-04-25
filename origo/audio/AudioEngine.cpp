#include <stdexcept>

#include "origo/audio/AudioEngine.h"

#include "origo/core/Logger.h"

namespace Origo {

AudioEngine::AudioEngine() {
	ma_result result;

	result = ma_engine_init(NULL, &m_miniaudio_engine);
	if (result != MA_SUCCESS) {
		throw std::runtime_error("Failed to initialize audio engine.");
	}
}

void AudioEngine::play_sound(const std::filesystem::path& path) {
	ma_engine_play_sound(&m_miniaudio_engine, path.c_str(), NULL);
}

std::optional<AudioEngine::PlaybackId> AudioEngine::create_sound(const std::filesystem::path& path, float volume, bool looping, float pitch) {
	auto instance = std::make_shared<SoundInstance>();
	instance->Path = path.string();

	const ma_uint32 flags = MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC | MA_SOUND_FLAG_STREAM;
	const ma_result result = ma_sound_init_from_file(&m_miniaudio_engine, instance->Path.c_str(), flags, NULL, NULL, &instance->Sound);
	if (result != MA_SUCCESS) {
		ORG_ERROR("AudioEngine: failed to initialize sound from '{}'", instance->Path);
		return std::nullopt;
	}

	ma_sound_set_volume(&instance->Sound, volume);
	ma_sound_set_looping(&instance->Sound, looping ? MA_TRUE : MA_FALSE);
	ma_sound_set_pitch(&instance->Sound, pitch);

	const PlaybackId id = m_next_playback_id++;
	m_sounds.emplace(id, std::move(instance));

	return id;
}

bool AudioEngine::has_sound(PlaybackId id) const {
	return m_sounds.contains(id);
}

void AudioEngine::resume() {
	ma_engine_start(&m_miniaudio_engine);
}

void AudioEngine::pause() {
	ma_engine_stop(&m_miniaudio_engine);
}

void AudioEngine::set_sound_volume(PlaybackId id, float volume) {
	auto it = m_sounds.find(id);
	if (it == m_sounds.end())
		return;

	ma_sound_set_volume(&it->second->Sound, volume);
}

void AudioEngine::set_sound_pitch(PlaybackId id, float pitch) {
	auto it = m_sounds.find(id);
	if (it == m_sounds.end())
		return;

	ma_sound_set_pitch(&it->second->Sound, pitch);
}

void AudioEngine::set_sound_looping(PlaybackId id, bool looping) {
	auto it = m_sounds.find(id);
	if (it == m_sounds.end())
		return;

	ma_sound_set_looping(&it->second->Sound, looping ? MA_TRUE : MA_FALSE);
}

void AudioEngine::set_listener_transform(const Vec3& position, const Vec3& direction, const Vec3& up) {
	ma_engine_listener_set_position(&m_miniaudio_engine, 0, position.x, position.y, position.z);
	ma_engine_listener_set_direction(&m_miniaudio_engine, 0, direction.x, direction.y, direction.z);
	ma_engine_listener_set_world_up(&m_miniaudio_engine, 0, up.x, up.y, up.z);
}

void AudioEngine::set_sound_spatial(PlaybackId id, bool is_spatial, const Vec3& position, float falloff_start_distance, float falloff_duration) {
	auto it = m_sounds.find(id);
	if (it == m_sounds.end())
		return;

	ma_sound_set_spatialization_enabled(&it->second->Sound, is_spatial ? MA_TRUE : MA_FALSE);
	if (!is_spatial)
		return;

	const float min_distance = std::max(0.0f, falloff_start_distance);
	const float max_distance = std::max(min_distance, min_distance + std::max(0.0f, falloff_duration));

	ma_sound_set_position(&it->second->Sound, position.x, position.y, position.z);
	ma_sound_set_attenuation_model(&it->second->Sound, ma_attenuation_model_linear);
	ma_sound_set_min_distance(&it->second->Sound, min_distance);
	ma_sound_set_max_distance(&it->second->Sound, max_distance);
}

bool AudioEngine::is_sound_playing(PlaybackId id) const {
	auto it = m_sounds.find(id);
	if (it == m_sounds.end())
		return false;

	return ma_sound_is_playing(&it->second->Sound) == MA_TRUE;
}

bool AudioEngine::is_sound_at_end(PlaybackId id) const {
	auto it = m_sounds.find(id);
	if (it == m_sounds.end())
		return true;

	return ma_sound_at_end(&it->second->Sound) == MA_TRUE;
}

void AudioEngine::start_sound(PlaybackId id) {
	auto it = m_sounds.find(id);
	if (it == m_sounds.end())
		return;

	ma_sound_seek_to_pcm_frame(&it->second->Sound, 0);
	ma_sound_start(&it->second->Sound);
}

void AudioEngine::stop_sound(PlaybackId id) {
	auto it = m_sounds.find(id);
	if (it == m_sounds.end())
		return;

	ma_sound_stop(&it->second->Sound);
}

void AudioEngine::destroy_sound(PlaybackId id) {
	auto it = m_sounds.find(id);
	if (it == m_sounds.end())
		return;

	ma_sound_uninit(&it->second->Sound);
	m_sounds.erase(it);
}

void AudioEngine::destroy_all_sounds() {
	for (auto& [_, sound] : m_sounds) {
		ma_sound_uninit(&sound->Sound);
	}
	m_sounds.clear();
}

AudioEngine::~AudioEngine() {
	destroy_all_sounds();

	ma_engine_uninit(&m_miniaudio_engine);
}

}
