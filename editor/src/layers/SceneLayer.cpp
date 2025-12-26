#include "layer/SceneLayer.h"

#include "components/FallComponent.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/Material.h"
#include "origo/assets/ShaderSource.h"
#include "origo/assets/Texture2D.h"
#include "origo/assets/PrimitiveShape.h"
#include "origo/assets/TextureSource.h"
#include "origo/core/Time.h"
#include "origo/renderer/GeometryHeapRegistry.h"
#include "origo/assets/Mesh.h"
#include "origo/renderer/VertexAttribute.h"
#include "origo/renderer/VertexLayoutRegistry.h"
#include "components/EditorSelection.h"
#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Transform.h"
#include <random>

using namespace Origo;

namespace OrigoEditor {
SceneLayer::SceneLayer(EditorContext& ctx)
    : m_Context(ctx) { }

void SceneLayer::OnAttach() {
	m_Texture = AssetFactory::CreateAsset<Texture2D>("Origo");
	AssetManagerFast::GetInstance().Get<Texture2D>(m_Texture)->SetSource(MakeScope<TextureSourceFile>("resources/textures/origo_logo.png"));

	m_Shader = AssetFactory::CreateAsset<Shader>("Normal Shader");
	AssetManagerFast::GetInstance().Get<Shader>(m_Shader)->SetSource(MakeScope<ShaderSourceFile>("resources/shaders/normal.glsl"));

	SpawnGrid(5);
}

void SceneLayer::OnUpdate(double dt) {
	auto shader { AssetManagerFast::GetInstance().Get<Shader>(m_Shader) };

	shader->UseProgram();

	float time = Time::GetTimeSinceStart().count();
	shader->SetUniform("u_Time", time);

	shader->SetUniform("u_ScreenSize", glm::vec2(m_Context.RenderBuffer.GetWidth(), m_Context.RenderBuffer.GetHeight()));
}

void SceneLayer::SpawnGrid(int gridSize, float spacing) {
	VertexLayout layout {};
	layout.AddAttribute<float>(3, false, VertexAttributeSemantic::Position);
	layout.AddAttribute<float>(3, false, VertexAttributeSemantic::Normal);
	layout.AddAttribute<float>(2, false, VertexAttributeSemantic::TexCoord);

	std::mt19937 random { std::random_device {}() };
	std::uniform_int_distribution<int> chanceDist { 0, 3 };
	std::uniform_real_distribution<float> speedDistr { 1.0f, 10.0f };

	int layoutId
	    = VertexLayoutRegistry::Register(layout);
	int heapId = GeometryHeapRegistry::CreateHeap(
	    layoutId,
	    GL_STATIC_DRAW,
	    120'000,
	    120'000);

	auto heap = GeometryHeapRegistry::GetHeap(heapId);

	MeshData data = GetDataFromShape(PrimitiveShape::Cube);
	auto range = heap->Allocate(
	    data.Vertices.data(),
	    data.Vertices.size() * sizeof(float),
	    3 * 3 * 2 * sizeof(float),
	    data.Indices.data(),
	    data.Indices.size());

	auto cubeMesh = AssetFactory::CreateAsset<Mesh>(
	    "GridCube",
	    layoutId,
	    heapId,
	    range);

	auto material = AssetFactory::CreateAsset<Material2D>(
	    "GridMaterial",
	    m_Shader,
	    m_Texture);

	const float half = (gridSize - 1) * spacing * 0.5f;

	int id = 0;

	for (int x = 0; x < gridSize; ++x) {
		for (int z = 0; z < gridSize; ++z) {
			glm::vec3 pos {
				x * spacing - half,
				0.0f,
				z * spacing - half
			};

			auto entity = m_Context.EditorScene->CreateEntity(
			    "GridCube_" + std::to_string(id++));

			auto transform = m_Context.EditorScene->AddNativeComponent<Transform>(entity);
			transform->SetPosition(pos);

			m_Context.EditorScene->AddNativeComponent<MeshRenderer>(
			    entity,
			    material,
			    cubeMesh);

			m_Context.EditorScene->AddNativeComponent<EditorSelection>(entity);
			if (!chanceDist(random))
				m_Context.EditorScene->AddNativeComponent<FallComponent>(entity, speedDistr(random));
		}
	}
}

}
