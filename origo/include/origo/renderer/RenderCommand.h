#include "origo/renderer/Material.h"
#include "origo/renderer/Mesh.h"
#include "origo/renderer/Transform.h"

namespace Origo {
class RenderCommand {
public:
	RenderCommand(Ref<Mesh> mesh, Ref<Material> material, Ref<Transform> transform);

	Ref<Mesh> GetMesh() const;
	Ref<Material> GetMaterial() const;
	Ref<Transform> GetTransform() const;

private:
	Ref<Mesh> m_Mesh {};
	Ref<Material> m_Material;
	Ref<Transform> m_Transform;
};
}
