#include <algorithm>
#include <utility>

#include "origo/core/LayerSystem.h"

#include "origo/core/Logger.h"

namespace Origo {

LayerSystem::LayerSystem() { }

LayerSystem::~LayerSystem() = default;

void LayerSystem::request_push_layer(Scope<Layer> layer, size_t key, bool frozen) {
	m_commands.push_back({ LayerCommandType::Push,
	    key,
	    std::move(layer),
	    frozen });
}

void LayerSystem::request_remove_layer(size_t key) {
	m_commands.push_back({ LayerCommandType::Remove,
	    key,
	    Scope<Layer> {},
	    false });
}

void LayerSystem::update_all(float dt) {
	for (auto i { 0uz }; i < m_layers.size(); i++) {
		m_layers[i]->on_update(dt);
	}

	while (!m_one_runners.empty()) {
		m_one_runners.top().second();
		freeze_layer(m_one_runners.top().first);
		m_one_runners.pop();
	}

	flush_commands();
}

void LayerSystem::request_freeze_layer(size_t key) {
	m_commands.push_back({ LayerCommandType::Freeze,
	    key,
	    Scope<Layer> {},
	    false });
}

void LayerSystem::request_activate_layer(size_t key, std::optional<std::function<void()>> runOnce) {
	LayerCommand to_push { LayerCommandType::Activate,
		key,
		Scope<Layer> {},
		false };

	if (runOnce.has_value()) {
		to_push.OneRunner = runOnce;
	}

	m_commands.push_back(std::move(to_push));
}

void LayerSystem::push_layer(Scope<Layer> layer, size_t key, bool frozen) {
	if (!layer) {
		ORG_CORE_ERROR("LayerSystem::PushLayer: Null layer");
		return;
	}

	auto& keys = frozen ? m_frozen_keys : m_keys;
	auto& layers = frozen ? m_frozen_layers : m_layers;
	auto& ptrs = frozen ? m_frozen_layer_ptrs : m_layer_ptrs;

	auto it = std::lower_bound(keys.begin(), keys.end(), key);

	if (it != keys.end() && *it == key) {
		ORG_CORE_ERROR("LayerSystem::PushLayer: Duplicate key {}", key);
		return;
	}

	size_t idx = std::distance(keys.begin(), it);
	Layer* raw_layer = layer.get();
	keys.insert(it, key);
	layers.insert(layers.begin() + idx, std::move(layer));
	ptrs.insert(ptrs.begin() + idx, raw_layer);
}

void LayerSystem::remove_layer(size_t key) {
	auto it = std::lower_bound(m_keys.begin(), m_keys.end(), key);

	if (it == m_keys.end() || *it != key) {
		ORG_CORE_ERROR("LayerSystem::RemoveLayer: No layer with key {}", key);
		return;
	}

	size_t idx = std::distance(m_keys.begin(), it);

	m_keys.erase(it);
	m_layer_ptrs.erase(m_layer_ptrs.begin() + idx);
	m_layers.erase(m_layers.begin() + idx);
}

void LayerSystem::freeze_layer(size_t key) {
	auto it = std::lower_bound(m_keys.begin(), m_keys.end(), key);

	if (it == m_keys.end() || *it != key) {
		ORG_CORE_ERROR("LayerSystem::FreezeLayer: No layer with key {}", key);
		return;
	}

	size_t idx = std::distance(m_keys.begin(), it);
	Scope<Layer> layer = std::move(m_layers[idx]);
	Layer* raw_layer = layer.get();

	raw_layer->on_detach();

	m_keys.erase(it);
	m_layer_ptrs.erase(m_layer_ptrs.begin() + idx);
	m_layers.erase(m_layers.begin() + idx);

	auto fit = std::lower_bound(m_frozen_keys.begin(), m_frozen_keys.end(), key);
	size_t fidx = std::distance(m_frozen_keys.begin(), fit);

	m_frozen_keys.insert(fit, key);
	m_frozen_layers.insert(m_frozen_layers.begin() + fidx, std::move(layer));
	m_frozen_layer_ptrs.insert(m_frozen_layer_ptrs.begin() + fidx, raw_layer);
}

void LayerSystem::activate_layer(size_t key, std::optional<std::function<void()>> runOnce) {
	auto it = std::lower_bound(m_frozen_keys.begin(), m_frozen_keys.end(), key);

	if (it == m_frozen_keys.end() || *it != key) {
		ORG_CORE_ERROR("LayerSystem::ActivateLayer: No frozen layer with key {}", key);
		return;
	}

	size_t idx = std::distance(m_frozen_keys.begin(), it);
	Scope<Layer> layer = std::move(m_frozen_layers[idx]);
	Layer* raw_layer = layer.get();

	m_frozen_keys.erase(it);
	m_frozen_layer_ptrs.erase(m_frozen_layer_ptrs.begin() + idx);
	m_frozen_layers.erase(m_frozen_layers.begin() + idx);

	auto ait = std::lower_bound(m_keys.begin(), m_keys.end(), key);
	size_t aidx = std::distance(m_keys.begin(), ait);

	m_keys.insert(ait, key);
	m_layers.insert(m_layers.begin() + aidx, std::move(layer));
	m_layer_ptrs.insert(m_layer_ptrs.begin() + aidx, raw_layer);

	raw_layer->on_attach();
	if (runOnce)
		m_one_runners.push({ key, *runOnce });
}

bool LayerSystem::has_active_layer(size_t key) const {
	return std::binary_search(m_keys.begin(), m_keys.end(), key);
}

void LayerSystem::flush_commands() {
	for (LayerCommand& cmd : m_commands) {
		switch (cmd.Type) {
		case LayerCommandType::Push:
			push_layer(std::move(cmd.OwnedLayer), cmd.Key, cmd.Frozen);
			break;

		case LayerCommandType::Remove:
			remove_layer(cmd.Key);
			break;

		case LayerCommandType::Freeze:
			freeze_layer(cmd.Key);
			break;

		case LayerCommandType::Activate:
			activate_layer(cmd.Key, cmd.OneRunner);
			break;
		}
	}

	m_commands.clear();
}

}
