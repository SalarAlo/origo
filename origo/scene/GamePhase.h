#pragma once

namespace Origo {

enum class GamePhase {
	UpdateGame,
	UpdateAlways,
	RenderAlways,
	RenderGame,
	RenderEditor,
};

};

namespace std {
template <>
struct hash<Origo::GamePhase> {
	size_t operator()(Origo::GamePhase phase) const noexcept {
		return static_cast<size_t>(phase);
	}
};
}
