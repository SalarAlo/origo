#include "origo/assets/Material.h"
#include "origo/assets/Mesh.h"
#include "origo/scene/Transform.h"

namespace Origo {
class RenderCommand {
public:
	RenderCommand(const Ref<Mesh>& mesh, const Ref<Material>& material, Transform* transform);

	Ref<Mesh> GetMesh() const;
	Ref<Material> GetMaterial() const;
	Transform* GetTransform() const;

private:
	Ref<Mesh> m_Mesh {};
	Ref<Material> m_Material;
	Transform* m_Transform;
};
}
