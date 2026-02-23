#include "origo/core/Init.h"

#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/PrimitiveShapeCache.h"

#include "origo/assets/importers/AssetImporterRegistry.h"
#include "origo/assets/importers/AssetPipeline.h"

#include "origo/assets/serialization/AssetSerializer.h"

#include "origo/scripting/ScriptSystem.h"

namespace Origo {
void Init() {
	Origo::Logger::Init();

	AssetSerializationSystem::RegisterAllAssetSerializers();
	AssetImporterRegistry::GetInstance().InitialiseDefaultImporters();
	AssetImportPipeline::RunInitialImport();

	DefaultAssetCache::GetInstance().CreateAllDefaults();
	PrimitiveShapeCache::GetInstance().CreateAllPrimitiveShapes();

	ScriptSystem::ReloadAll();

	ORG_INFO("OpenGL Version {}", (const char*)(glGetString(GL_VERSION)));
	ORG_INFO("ImGUI version: {} ({})", IMGUI_VERSION, IMGUI_VERSION_NUM);
}
}
