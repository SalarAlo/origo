#include "layer/SceneLayer.h"

#include "origo/assets/AssetFactory.h"
#include "origo/assets/Material.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/ShaderSource.h"
#include "origo/assets/Texture.h"
#include "origo/assets/PrimitiveShape.h"
#include "origo/assets/TextureSource.h"
#include "origo/core/Time.h"
#include "origo/renderer/GeometryHeapRegistry.h"
#include "origo/assets/Mesh.h"
#include "origo/renderer/VertexAttribute.h"
#include "origo/renderer/VertexLayoutRegistry.h"
#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Transform.h"

using namespace Origo;
static std::hash<RID> HashEntity {};

namespace OrigoEditor {
SceneLayer::SceneLayer(EditorContext& ctx)
    : m_Context(ctx) { }

void SceneLayer::OnAttach() {
	m_Texture = AssetFactory::CreateAsset<Texture>("Rowlett");
	AssetManager::GetAssetAs<Texture>(m_Texture)->SetSource(MakeScope<TextureSourceFile>("resources/textures/dirt.jpg"));

	m_Shader = AssetFactory::CreateAsset<Shader>("Normal Shader");
	AssetManager::GetAssetAs<Shader>(m_Shader)->SetSource(MakeScope<ShaderSourceFile>("resources/shaders/normal.glsl"));

	SpawnTestGrid();
}

void SceneLayer::OnEvent(Event& e) {
}

void SceneLayer::OnUpdate(double dt) {
	auto shader { AssetManager::GetAssetAs<Shader>(m_Shader) };
	shader->UseProgram();

	float time = Time::GetTimeSinceStart().count();
	shader->SetUniform("u_Time", time);

	int selectedEntityId { -1 };
	if (m_Context.SelectedEntity != nullptr) {
		selectedEntityId = static_cast<int>(HashEntity(m_Context.SelectedEntity->GetId()));
	}

	shader->SetUniform("u_SelectedEntityId", selectedEntityId);
	shader->SetUniform("u_ScreenSize", glm::vec2(m_Context.Buffer.GetWidth(), m_Context.Buffer.GetHeight()));
}

void SceneLayer::SpawnTestGrid() {
	VertexLayout layout {};
	layout.AddAttribute<float>(3, false, VertexAttributeSemantic::Position);
	layout.AddAttribute<float>(3, false, VertexAttributeSemantic::Normal);
	layout.AddAttribute<float>(2, false, VertexAttributeSemantic::TexCoord);

	int layoutId = VertexLayoutRegistry::Register(layout);
	int heapId { GeometryHeapRegistry::CreateHeap(layoutId, GL_STATIC_DRAW, 100'000, 100'000) };
	auto heap { GeometryHeapRegistry::GetHeap(heapId) };

	MeshData data = GetDataFromShape(PrimitiveShape::Cube);
	auto range = heap->Allocate(
	    data.Vertices.data(),
	    data.Vertices.size() * sizeof(float),
	    3 * 3 * 2 * sizeof(float),
	    data.Indices.data(),
	    data.Indices.size());

	auto cubeMesh = AssetFactory::CreateAsset<Mesh>("Cube", layoutId, heapId, range);

	auto materialId {
		AssetFactory::CreateAsset<Material>("Cube_Material", m_Shader, m_Texture)
	};

	auto material { AssetManager::GetAssetAs<Material>(materialId) };

	for (int i = 0; i < GRID_SIZE; ++i) {
		for (int j = 0; j < GRID_SIZE; ++j) {
			auto entity = m_Context.Scene.CreateEntity("Cube_" + std::to_string(i * GRID_SIZE + j));
			auto transform = m_Context.Scene.AddComponent<Transform>(entity);

			transform->SetPosition(glm::vec3 { i * 2, 0, j * 2 });
			transform->SetScale(glm::vec3 { 0.7f });

			m_Context.Scene.AddComponent<MeshRenderer>(entity, materialId, cubeMesh);
		}
	}
}

}
