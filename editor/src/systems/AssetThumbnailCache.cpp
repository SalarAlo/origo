#include "origo/assets/Texture2D.h"

using namespace Origo;

static ImTextureID to_im_texture_id(const Ref<Texture2D>& tex) {
	return (ImTextureID)(intptr_t)tex->get_renderer_id();
}

static Ref<Texture2D> load_svg_texture(const std::string& path, int size = 18) {
	auto texture = make_ref<Texture2D>(TextureType::UI);
	texture->set_source(make_scope<TextureSourceSVG>(path, size, size));
	texture->load();
	return texture;
}
