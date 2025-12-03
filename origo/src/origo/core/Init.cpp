#include "origo/core/Init.h"
#include "origo/assets/AssetSerializer.h"
#include "origo/assets/importers/AssetImporterRegistry.h"
#include "origo/assets/importers/AssetPipeline.h"

namespace Origo {
void Init() {
	Origo::Logger::Init();

	AssetSerializationSystem::RegisterAllAssetSerializers();
	AssetImporterRegistry::InitialiseDefaultImporters();
	AssetPipeline::RunInitialImport();

	ORG_INFO("OpenGL Version {}", (const char*)(glGetString(GL_VERSION)));
}
}
