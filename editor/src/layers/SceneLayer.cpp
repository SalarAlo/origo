#include "layer/SceneLayer.h"

#include "origo/assets/AssetManager.h"
#include "origo/renderer/Renderer.h"
#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Transform.h"

using namespace Origo;

namespace OrigoEditor {
SceneLayer::SceneLayer(EditorContext& ctx)
    : m_Context(ctx) { }

void SceneLayer::OnAttach() {
	auto logoTexture = AssetManager::CreateAsset<Texture>("Rowlett", "resources/textures/origo_logo.png");
	m_Shader = AssetManager::CreateAsset<Shader>("Normal Shader", "normal");
	m_Material = AssetManager::CreateAsset<Material>("Normal Material", m_Shader, logoTexture);

	SpawnTestGrid();
}

void SceneLayer::OnEvent(Event& e) {
}

void SceneLayer::OnUpdate(double dt) {
	static std::hash<UUID> HashEntity {};

	m_Context.Buffer.Bind();

	auto selectedEntityId { -1 };
	if (m_Context.SelectedEntity) {
		selectedEntityId = static_cast<int>(HashEntity(m_Context.SelectedEntity->GetId()));
	}
	m_Shader->SetUniform("u_SelectedEntityId", selectedEntityId);

	Renderer::SetViewport(m_Context.Buffer.GetWidth(), m_Context.Buffer.GetHeight());

	GLCall(glClearColor(0.1f, 0.1f, 0.12f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	Renderer::Flush(m_Context.Scene.GetMainCamera());

	m_Context.Buffer.Unbind();
}

void SceneLayer::SpawnTestGrid() {
	auto cubeMesh = AssetManager::CreateAsset<Mesh>("Cube", PrimitiveShape::Cube);

	for (int i = 0; i < GRID_SIZE; ++i) {
		for (int j = 0; j < GRID_SIZE; ++j) {
			auto entity = m_Context.Scene.CreateEntity("Cube_" + std::to_string(i * GRID_SIZE + j));
			auto transform = m_Context.Scene.AddComponent<Transform>(entity);
			transform->SetPosition(glm::vec3 { i * 2, 0, j * 2 });
			m_Context.Scene.AddComponent<MeshRenderer>(entity, m_Material, cubeMesh);
		}
	}
}
}
