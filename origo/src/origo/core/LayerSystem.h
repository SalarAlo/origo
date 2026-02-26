#pragma once

#include "origo/core/Layer.h"
#include <optional>
#include <stack>
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
		std::optional<std::function<void()>> OneRunner { std::nullopt };
	};

public:
	LayerSystem();
	~LayerSystem();

	void flush_commands();

	void request_push_layer(Layer* layer, size_t key, bool frozen = false);
	void request_remove_layer(size_t key);
	void request_freeze_layer(size_t key);
	void request_activate_layer(size_t key, std::optional<std::function<void()>> runOnce = std::nullopt);

	void update_all(float dt);

	bool has_active_layer(size_t key) const;

	std::vector<Layer*>::iterator begin() { return m_layers.begin(); }
	std::vector<Layer*>::iterator end() { return m_layers.end(); }

private:
	void push_layer(Layer* layer, size_t key, bool frozen = false);
	void remove_layer(size_t key);
	void freeze_layer(size_t key);
	void activate_layer(size_t key, std::optional<std::function<void()>> runOnc);

private:
	std::vector<Layer*> m_layers {};
	std::vector<size_t> m_keys {};

	std::vector<Layer*> m_frozen_layers {};
	std::vector<size_t> m_frozen_keys {};
	std::stack<std::pair<size_t, std::function<void()>>> m_one_runners {};

	std::vector<LayerCommand> m_commands {};
};

}
