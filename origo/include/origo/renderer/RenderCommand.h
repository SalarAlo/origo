#include "origo/core/UUID.h"
#include "origo/scene/Transform.h"

namespace Origo {
class RenderCommand {
public:
	RenderCommand(const UUID& mesh, const UUID& material, Transform* transform);

	UUID GetMesh() const;
	UUID GetMaterial() const;
	Transform* GetTransform() const;

private:
	UUID m_Mesh {};
	UUID m_Material;
	Transform* m_Transform;
};
}
