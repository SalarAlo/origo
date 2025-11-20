#include "origo/scene/Transform.h"

namespace Origo {
class RenderCommand {
public:
	RenderCommand(const RID& mesh, const RID& material, Transform* transform);

	RID GetMesh() const;
	RID GetMaterial() const;
	Transform* GetTransform() const;

private:
	RID m_Mesh {};
	RID m_Material;
	Transform* m_Transform;
};
}
