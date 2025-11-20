#pragma once

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
	void SpawnTestGrid();

private:
	static constexpr int GRID_SIZE { 4 };
	EditorContext& m_Context;

	Origo::RID m_Shader;
	Origo::RID m_Texture;
};

}
