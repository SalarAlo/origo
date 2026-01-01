#include "origo/core/LayerSystem.h"
#include "origo/core/Logger.h"

namespace Origo {

LayerSystem::LayerSystem() {
}

LayerSystem::~LayerSystem() {
	for (Layer* layer : m_Layers)
		delete layer;
}

void LayerSystem::PushLayer(Layer* layer, size_t activeKey) {
	for (const auto& key : m_Keys) {
		if (key == activeKey) {
			ORG_ERROR("LayerSystem::PushLayer: Already contain layer with key {}", key);
			return;
		}
	}

	m_Layers.push_back(layer);
	m_Keys.push_back(activeKey);
}

void LayerSystem::RemoveLayer(size_t key) {
	auto keyIdx { 0uz };
	for (; keyIdx < m_Keys.size(); keyIdx++) {
		if (key == m_Keys[keyIdx])
			break;
	}

	if (key == m_Keys.size()) {
		ORG_ERROR("LayerStack::PopLayer: No Layer with key {}", key);
	}
	auto layer { m_Layers[keyIdx] };

	m_Layers.erase(m_Layers.begin() + keyIdx);
	m_Keys.erase(m_Keys.begin() + keyIdx);

	delete layer;
}

void LayerSystem::FreezeLayer(size_t key) {
	auto keyIdx { 0uz };
	for (; keyIdx < m_Keys.size(); keyIdx++) {
		if (key == m_Keys[keyIdx])
			break;
	}

	if (key == m_Keys.size()) {
		ORG_ERROR("LayerStack::FreezeLayer: No Layer with key {}", key);
	}

	auto layer { m_Layers[keyIdx] };
	auto layerKey { m_Keys[keyIdx] };
	layer->OnDetach();

	m_Layers.erase(m_Layers.begin() + keyIdx);
	m_Keys.erase(m_Keys.begin() + keyIdx);

	m_FrozenLayers.push_back(layer);
	m_FrozenKeys.push_back(layerKey);

	delete layer;
}

void LayerSystem::ActivateLayer(size_t key) {
	auto frozenKeyIdx { 0uz };
	for (; frozenKeyIdx < m_FrozenKeys.size(); frozenKeyIdx++) {
		if (key == m_FrozenKeys[frozenKeyIdx])
			break;
	}

	if (key == m_Keys.size()) {
		ORG_ERROR("LayerStack::FreezeLayer: No Layer with key {}", key);
	}

	auto layer { m_FrozenLayers[frozenKeyIdx] };

	auto keyIdx { 0uz };
	for (; keyIdx < m_Keys.size(); keyIdx++) {
		if (m_Keys[keyIdx] > key)
			break;
	}

	m_Layers.insert(m_Layers.begin() + keyIdx, layer);
	m_Keys.insert(m_Keys.begin() + keyIdx, key);

	m_FrozenLayers.erase(m_FrozenLayers.begin() + frozenKeyIdx);
	m_FrozenKeys.erase(m_FrozenKeys.begin() + frozenKeyIdx);

	delete layer;
}

}
