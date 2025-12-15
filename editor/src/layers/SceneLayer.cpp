#include "layer/SceneLayer.h"

#include "origo/assets/AssetFactory.h"
#include "origo/assets/Material.h"
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
	AssetManagerFast::GetInstance().Get<Texture>(m_Texture)->SetSource(MakeScope<TextureSourceFile>("resources/textures/rowlett.jpg"));

	m_Shader = AssetFactory::CreateAsset<Shader>("Normal Shader");
	AssetManagerFast::GetInstance().Get<Shader>(m_Shader)->SetSource(MakeScope<ShaderSourceFile>("resources/shaders/normal.glsl"));

	SpawnRing();
}

void SceneLayer::OnEvent(Event& e) {
}

void SceneLayer::OnUpdate(double dt) {
	auto shader { AssetManagerFast::GetInstance().Get<Shader>(m_Shader) };
	shader->UseProgram();

	float time = Time::GetTimeSinceStart().count();
	shader->SetUniform("u_Time", time);

	int selectedEntityId { -1 };
	if (m_Context.SelectedEntity != nullptr) {
		selectedEntityId = static_cast<int>(HashEntity(m_Context.SelectedEntity->GetId()));
	}

	shader->SetUniform("u_SelectedEntityId", selectedEntityId);
	shader->SetUniform("u_ScreenSize", glm::vec2(m_Context.RenderBuffer.GetWidth(), m_Context.RenderBuffer.GetHeight()));
}

void SceneLayer::SpawnRing() {
	VertexLayout layout {};
	layout.AddAttribute<float>(3, false, VertexAttributeSemantic::Position);
	layout.AddAttribute<float>(3, false, VertexAttributeSemantic::Normal);
	layout.AddAttribute<float>(2, false, VertexAttributeSemantic::TexCoord);

	int layoutId = VertexLayoutRegistry::Register(layout);
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
	    "RingCube",
	    layoutId,
	    heapId,
	    range);

	auto material = AssetFactory::CreateAsset<Material>(
	    "RingMaterial",
	    m_Shader,
	    m_Texture);

	constexpr int RING_COUNT { 9 };
	constexpr int SEGMENTS { 64 };
	constexpr float BASE_RADIUS { 6.0f };
	constexpr float PI { 3.1416 };

	int id {};

	for (int r = 0; r < RING_COUNT; ++r) {
		float radius = BASE_RADIUS + r * 1.2f;
		float height = (r - RING_COUNT * 0.5f) * 0.8f;
		float tilt = r * 0.25f;

		for (int i = 0; i < SEGMENTS; ++i) {
			float a = (float)i / (float)SEGMENTS * 2.0f * PI;

			glm::vec3 pos {
				std::cos(a) * radius,
				height,
				std::sin(a) * radius
			};

			// Helical distortion for visual interest
			pos.y += std::sin(a * 3.0f + r) * 0.4f;

			auto entity = m_Context.Scene.CreateEntity(
			    "Ring_" + std::to_string(id++));

			auto transform = m_Context.Scene.AddComponent<Transform>(entity);
			transform->SetPosition(pos);
			transform->SetScale(glm::vec3 { 0.6f, 1.8f, 0.6f });
			transform->SetRotation(glm::vec3 { tilt, a, 0.0f });

			m_Context.Scene.AddComponent<MeshRenderer>(
			    entity,
			    material,
			    cubeMesh);
		}
	}
}
}
