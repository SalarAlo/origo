#include "origo/assets/Material.h"
#include "origo/assets/Mesh.h"
#include "origo/scene/Transform.h"

namespace Origo {
class RenderCommand {
public:
	RenderCommand(const Ref<Mesh>& mesh, const Ref<Material>& material, const Ref<Transform>& transform);

	Ref<Mesh> GetMesh() const;
	Ref<Material> GetMaterial() const;
	Ref<Transform> GetTransform() const;

private:
	WeakRef<Mesh> m_Mesh {};
	WeakRef<Material> m_Material;
	WeakRef<Transform> m_Transform;
};
}
