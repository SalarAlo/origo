#pragma once

namespace Origo {

enum class GamePhase {
	Update, // only play runs
	UpdatePresentation, // runs always
	RenderGeometry,
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
