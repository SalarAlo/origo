#pragma once

#include "origo/renderer/Texture.h"

namespace Origo::TextureCache {
Ref<Texture> Load(const std::string& path);
void Clear();
}
