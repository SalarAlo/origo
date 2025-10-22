#pragma once

#include "origo/assets/Asset.h"
#include "origo/assets/Mesh.h"

namespace Origo {

class Model : public Asset {
public:
	Model(std::string_view path);
	std::string GetPath() const { return m_Path; }

private:
	std::vector<Ref<Mesh>> m_Meshes;

	std::string m_Path;
};

}
