#include "origo/assets/AssetManagerFast.h"
#include "origo/scene/Transform.h"

namespace Origo {
class RenderCommand {
public:
	RenderCommand(const AssetHandle& mesh, const AssetHandle& material, Transform* transform);

	AssetHandle GetMesh() const;
	AssetHandle GetMaterial() const;
	Transform* GetTransform() const;

private:
	AssetHandle m_Mesh;
	AssetHandle m_Material;
	Transform* m_Transform;
};
}
