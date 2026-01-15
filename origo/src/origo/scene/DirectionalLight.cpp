#include "DirectionalLight.h"
#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetManager.h"
#include "origo/scene/NativeComponentRegistry.h"

ORIGO_REGISTER_NATIVE_COMPONENT(Origo::DirectionalLight)

namespace Origo {

void DirectionalLight::Serialize(ISerializer& backend) const {
	backend.Write("ambient", m_AmbientFactor);
	backend.Write("intensity", m_Intensity);
	backend.Write("color_r", m_LightColor.r);
	backend.Write("color_g", m_LightColor.g);
	backend.Write("color_b", m_LightColor.b);

	if (!m_ShaderTarget.has_value())
		return;

	auto id { AssetManager::GetInstance().GetUUID(*m_ShaderTarget) };
	if (!id.has_value())
		return;

	backend.Write("target", id->ToString());
}

void DirectionalLight::Deserialize(ISerializer& backend) {
	backend.TryRead("ambient", m_AmbientFactor);
	backend.TryRead("intensity", m_Intensity);

	backend.TryRead("color_r", m_LightColor.r);
	backend.TryRead("color_g", m_LightColor.g);
	backend.TryRead("color_b", m_LightColor.b);

	std::string targetId;
	if (!backend.TryRead("target", targetId))
		return;

	auto uuid = UUID::FromString(targetId);

	auto asset = AssetManager::GetInstance().GetHandleByUUID(uuid);
	if (!asset.has_value())
		return;

	m_ShaderTarget = asset;
}
}
