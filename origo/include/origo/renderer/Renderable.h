#include "origo/renderer/Material.h"
#include "origo/renderer/Mesh.h"

#include <memory>

namespace Origo {
class Renderable {
public:
	Renderable(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> m_Material);
	std::shared_ptr<Mesh> GetMesh() const;
	std::shared_ptr<Material> GetMaterial() const;

private:
	std::shared_ptr<Mesh> m_Mesh {};
	std::shared_ptr<Material> m_Material;
};
}
