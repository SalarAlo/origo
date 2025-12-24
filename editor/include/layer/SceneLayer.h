#pragma once

#include "origo/assets/AssetManagerFast.h"
#include "origo/core/Layer.h"

#include "state/EditorContext.h"

namespace OrigoEditor {

class SceneLayer : public Origo::Layer {
public:
	SceneLayer(EditorContext& ctx);

	void OnAttach() override;
	void OnUpdate(double dt) override;

private:
	void SpawnGrid(int gridSize = 10, float spacing = 1.5f);

private:
	EditorContext& m_Context;

	Origo::AssetHandle m_Shader {};
	Origo::AssetHandle m_OutlineShader {};
	Origo::AssetHandle m_Texture {};
};

}
