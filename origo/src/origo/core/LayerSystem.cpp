#include "origo/core/LayerSystem.h"
#include "origo/core/Logger.h"

#include <algorithm>

namespace Origo {

LayerSystem::LayerSystem() { }

LayerSystem::~LayerSystem() {
	for (Layer* layer : m_Layers)
		delete layer;

	for (Layer* layer : m_FrozenLayers)
		delete layer;
}

void LayerSystem::RequestPushLayer(Layer* layer, size_t key, bool frozen) {
	m_Commands.push_back({ LayerCommandType::Push,
	    key,
	    layer,
	    frozen });
}

void LayerSystem::RequestRemoveLayer(size_t key) {
	m_Commands.push_back({ LayerCommandType::Remove,
	    key,
	    nullptr,
	    false });
}

void LayerSystem::UpdateAll(float dt) {
	for (auto i { 0uz }; i < m_Layers.size(); i++) {
		m_Layers[i]->OnUpdate(dt);
	}

	while (!m_OneRunners.empty()) {
		m_OneRunners.top().second();
		FreezeLayer(m_OneRunners.top().first);
		m_OneRunners.pop();
	}

	FlushCommands();
}

void LayerSystem::RequestFreezeLayer(size_t key) {
	m_Commands.push_back({ LayerCommandType::Freeze,
	    key,
	    nullptr,
	    false });
}

void LayerSystem::RequestActivateLayer(size_t key, std::optional<std::function<void()>> runOnce) {
	LayerCommand toPush { LayerCommandType::Activate,
		key,
		nullptr,
		false };

	if (runOnce.has_value()) {
		toPush.OneRunner = runOnce;
	}

	m_Commands.push_back(toPush);
}

void LayerSystem::PushLayer(Layer* layer, size_t key, bool frozen) {
	auto& keys = frozen ? m_FrozenKeys : m_Keys;
	auto& layers = frozen ? m_FrozenLayers : m_Layers;

	auto it = std::lower_bound(keys.begin(), keys.end(), key);

	if (it != keys.end() && *it == key) {
		ORG_CORE_ERROR("LayerSystem::PushLayer: Duplicate key {}", key);
		delete layer;
		return;
	}

	size_t idx = std::distance(keys.begin(), it);
	keys.insert(it, key);
	layers.insert(layers.begin() + idx, layer);
}

void LayerSystem::RemoveLayer(size_t key) {
	auto it = std::lower_bound(m_Keys.begin(), m_Keys.end(), key);

	if (it == m_Keys.end() || *it != key) {
		ORG_CORE_ERROR("LayerSystem::RemoveLayer: No layer with key {}", key);
		return;
	}

	size_t idx = std::distance(m_Keys.begin(), it);
	Layer* layer = m_Layers[idx];

	m_Keys.erase(it);
	m_Layers.erase(m_Layers.begin() + idx);

	delete layer;
}

void LayerSystem::FreezeLayer(size_t key) {
	auto it = std::lower_bound(m_Keys.begin(), m_Keys.end(), key);

	if (it == m_Keys.end() || *it != key) {
		ORG_CORE_ERROR("LayerSystem::FreezeLayer: No layer with key {}", key);
		return;
	}

	size_t idx = std::distance(m_Keys.begin(), it);
	Layer* layer = m_Layers[idx];

	layer->OnDetach();

	m_Keys.erase(it);
	m_Layers.erase(m_Layers.begin() + idx);

	auto fit = std::lower_bound(m_FrozenKeys.begin(), m_FrozenKeys.end(), key);
	size_t fidx = std::distance(m_FrozenKeys.begin(), fit);

	m_FrozenKeys.insert(fit, key);
	m_FrozenLayers.insert(m_FrozenLayers.begin() + fidx, layer);
}

void LayerSystem::ActivateLayer(size_t key, std::optional<std::function<void()>> runOnce) {
	auto it = std::lower_bound(m_FrozenKeys.begin(), m_FrozenKeys.end(), key);

	if (it == m_FrozenKeys.end() || *it != key) {
		ORG_CORE_ERROR("LayerSystem::ActivateLayer: No frozen layer with key {}", key);
		return;
	}

	size_t idx = std::distance(m_FrozenKeys.begin(), it);
	Layer* layer = m_FrozenLayers[idx];

	m_FrozenKeys.erase(it);
	m_FrozenLayers.erase(m_FrozenLayers.begin() + idx);

	auto ait = std::lower_bound(m_Keys.begin(), m_Keys.end(), key);
	size_t aidx = std::distance(m_Keys.begin(), ait);

	m_Keys.insert(ait, key);
	m_Layers.insert(m_Layers.begin() + aidx, layer);

	layer->OnAttach();
	if (runOnce)
		m_OneRunners.push({ key, *runOnce });
}

bool LayerSystem::HasActiveLayer(size_t key) const {
	return std::binary_search(m_Keys.begin(), m_Keys.end(), key);
}

void LayerSystem::FlushCommands() {
	for (const LayerCommand& cmd : m_Commands) {
		switch (cmd.Type) {
		case LayerCommandType::Push:
			PushLayer(cmd.ActiveLayer, cmd.Key, cmd.Frozen);
			break;

		case LayerCommandType::Remove:
			RemoveLayer(cmd.Key);
			break;

		case LayerCommandType::Freeze:
			FreezeLayer(cmd.Key);
			break;

		case LayerCommandType::Activate:
			ActivateLayer(cmd.Key, cmd.OneRunner);
			break;
		}
	}

	m_Commands.clear();
}

}
