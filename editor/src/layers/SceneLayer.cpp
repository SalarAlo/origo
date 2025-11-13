#include "layer/SceneLayer.h"

#include "origo/assets/Material.h"
#include "origo/assets/AssetManager.h"
#include "origo/core/Time.h"
#include "origo/renderer/Renderer.h"
#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Transform.h"

using namespace Origo;
static std::hash<UUID> HashEntity {};

namespace OrigoEditor {
SceneLayer::SceneLayer(EditorContext& ctx)
    : m_Context(ctx) { }

void SceneLayer::OnAttach() {
	m_Texture = AssetManager::CreateAsset<Texture>("Rowlett", "resources/textures/origo_logo.png");
	m_Shader = AssetManager::CreateAsset<Shader>("Normal Shader", "normal");

	SpawnTestGrid();
}

void SceneLayer::OnEvent(Event& e) {
}

void SceneLayer::OnUpdate(double dt) {
	m_Context.Buffer.Bind();

	Renderer::SetViewport(m_Context.Buffer.GetWidth(), m_Context.Buffer.GetHeight());

	GLCall(glClearColor(0.1f, 0.1f, 0.12f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	Renderer::Flush(m_Context.Scene.GetMainCamera());
	float x = Time::GetTimeSinceStart().count();
	m_Shader->SetUniform("u_Time", x);

	int selectedEntityId { -1 };
	if (m_Context.SelectedEntity != nullptr) {
		selectedEntityId = static_cast<int>(HashEntity(m_Context.SelectedEntity->GetId()));
	}

	m_Shader->SetUniform("u_SelectedEntityId", selectedEntityId);
	m_Context.Buffer.Unbind();
}

void SceneLayer::SpawnTestGrid() {
	auto cubeMesh = AssetManager::CreateAsset<Mesh>("Cube", PrimitiveShape::Cube);

	for (int i = 0; i < GRID_SIZE; ++i) {
		for (int j = 0; j < GRID_SIZE; ++j) {
			auto entity = m_Context.Scene.CreateEntity("Cube_" + std::to_string(i * GRID_SIZE + j));
			auto transform = m_Context.Scene.AddComponent<Transform>(entity);
			transform->SetPosition(glm::vec3 { i * 2, 0, j * 2 });
			auto material {
				AssetManager::CreateAsset<Material>(
				    "Cube_Material_" + std::to_string(i * GRID_SIZE + j),
				    m_Shader,
				    m_Texture)
			};

			material->GetUniformList()->AddUniform("u_CurrentEntityId", static_cast<int>(HashEntity(entity->GetId())));

			m_Context.Scene.AddComponent<MeshRenderer>(entity, material, cubeMesh);
		}
	}
}
}
