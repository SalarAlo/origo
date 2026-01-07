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
	void CreateAssets();

private:
	EditorContext& m_Context;

	Origo::AssetHandle m_CubeMesh;

	Origo::AssetHandle m_ModelShader;
	Origo::AssetHandle m_Model;

	uint32_t m_VertexLayoutID = 0;
	uint32_t m_HeapID = 0;
	size_t m_VertexStride = 0;
};

}
