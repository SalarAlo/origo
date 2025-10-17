#pragma once

#include "origo/renderer/Material.h"
#include "origo/renderer/Shader.h"
#include "origo/renderer/Texture.h"

namespace Origo::MaterialLibrary {

Ref<Material> Create(Ref<Shader> shader, Ref<Texture> texture);

}
