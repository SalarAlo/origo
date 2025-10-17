#include "origo/assets/MaterialLibrary.h"
#include "origo/renderer/Material.h"

namespace Origo::MaterialLibrary {

Ref<Material> Create(Ref<Shader> shader, Ref<Texture> texture) {
	return MakeRef<Material>(shader, texture);
}

}
