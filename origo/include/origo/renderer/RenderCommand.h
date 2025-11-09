#include "origo/assets/Material.h"
#include "origo/assets/Mesh.h"
#include "origo/scene/Transform.h"

namespace Origo {
class RenderCommand {
public:
	RenderCommand(Mesh* mesh, Material* material, Transform* transform);

	Mesh* GetMesh() const;
	Material* GetMaterial() const;
	Transform* GetTransform() const;

private:
	Mesh* m_Mesh {};
	Material* m_Material;
	Transform* m_Transform;
};
}
