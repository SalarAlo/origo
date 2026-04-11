#include "origo/assets/material/MaterialColor.h"

#include "origo/assets/DefaultAssetCache.h"

namespace Origo {

MaterialColor::MaterialColor()
    : Material(DefaultAssetCache::get_instance().get_color_shader()) {
}

void MaterialColor::resolve() {
	resolve_material_base();

	if (!m_shader)
		ORG_CORE_ERROR("MaterialColor resolve failed: missing shader");
}

void MaterialColor::bind() {
	if (!m_shader)
		return;

	bind_shader_and_upload_uniforms();
	m_shader->set_uniform("u_unlit", m_data.Unlit);
}

}
