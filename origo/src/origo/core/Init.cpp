#include "origo/core/Init.h"

#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/PrimitiveShapeCache.h"

#include "origo/assets/importers/AssetImporterRegistry.h"
#include "origo/assets/importers/AssetPipeline.h"

#include "origo/assets/serialization/AssetSerializer.h"

#include "origo/scripting/ScriptSystem.h"

namespace Origo {
void init() {
	Origo::Logger::Init();

	AssetSerializationSystem::register_all_asset_serializers();
	AssetImporterRegistry::get_instance().initialise_default_importers();

	DefaultAssetCache::get_instance().create_all_defaults();
	PrimitiveShapeCache::get_instance().create_all_primitive_shapes();

	AssetImportPipeline::get_instance().run_initial_import();

	ScriptSystem::reload_all();

	ORG_CORE_INFO("OpenGL Version {}", (const char*)(glGetString(GL_VERSION)));
	ORG_CORE_INFO("ImGUI version: {} ({})", IMGUI_VERSION, IMGUI_VERSION_NUM);
}
}
