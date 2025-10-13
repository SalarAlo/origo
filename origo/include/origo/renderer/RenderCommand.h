#include "origo/renderer/Material.h"
#include "origo/renderer/Mesh.h"

namespace Origo {
class RenderCommand {
public:
	RenderCommand(Ref<Mesh> mesh, Ref<Material> m_Material);
	Ref<Mesh> GetMesh() const;
	Ref<Material> GetMaterial() const;

private:
	Ref<Mesh> m_Mesh {};
	Ref<Material> m_Material;
};
}
