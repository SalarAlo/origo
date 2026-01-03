#include "origo/assets/Texture2D.h"

using namespace Origo;

static ImTextureID ToImTextureID(const Ref<Texture2D>& tex) {
	return (ImTextureID)(intptr_t)tex->GetRendererID();
}

static Ref<Texture2D> LoadSVGTexture(const std::string& path, int size = 18) {
	auto texture = MakeRef<Texture2D>(TextureType::UI);
	texture->SetSource(MakeScope<TextureSourceSVG>(path, size, size));
	texture->Load();
	return texture;
}
