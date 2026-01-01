#pragma once

#include "origo/core/Layer.h"
#include <vector>

namespace Origo {

class LayerSystem {
public:
	LayerSystem();
	~LayerSystem();

	void PushLayer(Layer* layer, size_t key);
	void RemoveLayer(size_t key);
	void FreezeLayer(size_t key);
	void ActivateLayer(size_t key);

	std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
	std::vector<Layer*>::iterator end() { return m_Layers.end(); }

private:
	std::vector<Layer*> m_Layers {};
	std::vector<size_t> m_Keys {};

	std::vector<Layer*> m_FrozenLayers {};
	std::vector<size_t> m_FrozenKeys {};
};

}
