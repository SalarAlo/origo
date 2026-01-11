#include "origo/scene/MeshRenderer.h"
#include "origo/scene/NativeComponentRegistry.h"

namespace Origo {

MeshRenderer::MeshRenderer(OptionalAssetHandle material, OptionalAssetHandle mesh)
    : m_Material(material)
    , m_Mesh(mesh) {
}

ORIGO_REGISTER_NATIVE_COMPONENT(MeshRenderer)

void MeshRenderer::Serialize(ISerializer& backend) const {
	if (m_Material.has_value()) {
		auto uuid = AssetManager::GetInstance().GetUUID(*m_Material);
		if (uuid.has_value()) {
			backend.Write("material", uuid->ToString());
		}
	}

	if (m_Mesh.has_value()) {
		auto uuid = AssetManager::GetInstance().GetUUID(*m_Mesh);
		if (uuid.has_value()) {
			backend.Write("mesh", uuid->ToString());
		}
	}
}

void MeshRenderer::Deserialize(ISerializer& backend) {
	std::string materialId;
	if (backend.TryRead("material", materialId)) {
		UUID uuid = UUID::FromString(materialId);
		auto handle = AssetManager::GetInstance().GetHandleByUUID(uuid);
		if (handle.has_value()) {
			m_Material = handle;
		}
	}

	std::string meshId;
	if (backend.TryRead("mesh", meshId)) {
		UUID uuid = UUID::FromString(meshId);
		auto handle = AssetManager::GetInstance().GetHandleByUUID(uuid);
		if (handle.has_value()) {
			m_Mesh = handle;
		}
	}
}

}
