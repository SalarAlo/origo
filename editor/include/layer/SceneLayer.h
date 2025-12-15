#pragma once

#include "origo/assets/AssetManagerFast.h"
#include "origo/core/Layer.h"

#include "EditorContext.h"

namespace OrigoEditor {

class SceneLayer : public Origo::Layer {
public:
	SceneLayer(EditorContext& ctx);

	void OnAttach() override;
	void OnUpdate(double dt) override;
	void OnEvent(Origo::Event& e) override;

private:
	void SpawnRing();

private:
	EditorContext& m_Context;

	Origo::AssetHandle m_Shader {};
	Origo::AssetHandle m_Texture {};
};

}
