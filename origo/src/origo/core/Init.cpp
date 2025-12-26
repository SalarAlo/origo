#include "origo/core/Init.h"
#include "origo/assets/AssetSerializer.h"
#include "origo/assets/importers/AssetImporterRegistry.h"
#include "origo/assets/importers/AssetPipeline.h"
#include "origo/scripting/ScriptSystem.h"

namespace Origo {
void Init() {
	Origo::Logger::Init();

	AssetSerializationSystem::RegisterAllAssetSerializers();
	AssetImporterRegistry::InitialiseDefaultImporters();
	AssetPipeline::RunInitialImport();
	ScriptSystem::ReloadAll();

	ORG_INFO("OpenGL Version {}", (const char*)(glGetString(GL_VERSION)));
}
}
