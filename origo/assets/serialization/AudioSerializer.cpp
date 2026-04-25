#include <string>

#include "origo/assets/serialization/AudioSerializer.h"

#include "origo/assets/Audio.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {

void AudioSerializer::serialize(const Asset* asset, ISerializer& backend) const {
	auto audio { dynamic_cast<const Audio*>(asset) };

	if (audio->AudioPath) {
		backend.write("file_path", std::string(audio->AudioPath->c_str()));
	}
}

void AudioSerializer::deserialize(ISerializer& backend, Asset& asset) const {
	auto audio { dynamic_cast<Audio&>(asset) };

	if (std::string file_path {}; backend.try_read("file_path", file_path)) {
		audio.AudioPath = file_path;
	}
}

}
