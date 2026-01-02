#pragma once

#include "origo/core/Layer.h"
#include <vector>

namespace Origo {

class LayerSystem {
public:
	enum class LayerCommandType {
		Freeze,
		Activate,
		Remove,
		Push
	};

	struct LayerCommand {
		LayerCommandType Type;
		size_t Key;
		Layer* ActiveLayer;
		bool Frozen;
	};

public:
	LayerSystem();
	~LayerSystem();

	void FlushCommands();

	void RequestPushLayer(Layer* layer, size_t key, bool frozen = false);
	void RequestRemoveLayer(size_t key);
	void RequestFreezeLayer(size_t key);
	void RequestActivateLayer(size_t key);

	bool HasActiveLayer(size_t key) const;

	std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
	std::vector<Layer*>::iterator end() { return m_Layers.end(); }

private:
	void PushLayer(Layer* layer, size_t key, bool frozen = false);
	void RemoveLayer(size_t key);
	void FreezeLayer(size_t key);
	void ActivateLayer(size_t key);

private:
	std::vector<Layer*> m_Layers {};
	std::vector<size_t> m_Keys {};

	std::vector<Layer*> m_FrozenLayers {};
	std::vector<size_t> m_FrozenKeys {};

	std::vector<LayerCommand> m_Commands {};
};

}
