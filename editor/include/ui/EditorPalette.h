#pragma once

namespace OrigoEditor {

struct EditorPalette {
	std::string Name {};

	ImVec4 Surface0;
	ImVec4 Surface1;
	ImVec4 Surface2;

	ImVec4 Hover;
	ImVec4 Active;

	ImVec4 Accent;
	ImVec4 AccentSoft;
	ImVec4 AccentStrong;

	ImVec4 Text;
	ImVec4 TextDim;

	ImVec4 Border;
	ImVec4 BorderStrong;

	ImVec4 Selection;
	ImVec4 Error;
};

inline EditorPalette GetDefaultEditorPalette() {
	return {
		"Default",
		{ 0.090f, 0.090f, 0.090f, 1.00f },
		{ 0.115f, 0.115f, 0.115f, 1.00f },
		{ 0.155f, 0.155f, 0.155f, 1.00f },

		{ 0.240f, 0.240f, 0.240f, 1.00f },
		{ 0.310f, 0.310f, 0.310f, 1.00f },

		{ 0.22f, 0.45f, 0.78f, 1.00f },
		{ 0.18f, 0.34f, 0.55f, 1.00f },
		{ 0.30f, 0.55f, 0.90f, 1.00f },

		{ 0.88f, 0.88f, 0.88f, 1.00f },
		{ 0.52f, 0.52f, 0.52f, 1.00f },

		{ 0.06f, 0.06f, 0.06f, 1.00f },
		{ 0.12f, 0.12f, 0.12f, 1.00f },

		{ 0.18f, 0.34f, 0.55f, 0.35f },
		{ 0.82f, 0.25f, 0.25f, 1.00f }
	};
}

inline EditorPalette GetGraphiteEditorPalette() {
	return {
		"Graphite",
		{ 0.085f, 0.087f, 0.090f, 1.00f },
		{ 0.110f, 0.113f, 0.118f, 1.00f },
		{ 0.145f, 0.148f, 0.153f, 1.00f },

		{ 0.220f, 0.225f, 0.230f, 1.00f },
		{ 0.290f, 0.295f, 0.300f, 1.00f },

		{ 0.50f, 0.62f, 0.72f, 1.00f },
		{ 0.38f, 0.48f, 0.56f, 1.00f },
		{ 0.60f, 0.74f, 0.88f, 1.00f },

		{ 0.90f, 0.90f, 0.90f, 1.00f },
		{ 0.58f, 0.58f, 0.58f, 1.00f },

		{ 0.07f, 0.07f, 0.07f, 1.00f },
		{ 0.14f, 0.14f, 0.14f, 1.00f },

		{ 0.38f, 0.52f, 0.64f, 0.35f },
		{ 0.85f, 0.28f, 0.28f, 1.00f }
	};
}

inline EditorPalette GetNordSteelEditorPalette() {
	return {
		"Nord Steel",
		{ 0.070f, 0.085f, 0.100f, 1.00f },
		{ 0.095f, 0.110f, 0.125f, 1.00f },
		{ 0.130f, 0.145f, 0.160f, 1.00f },

		{ 0.200f, 0.220f, 0.240f, 1.00f },
		{ 0.270f, 0.290f, 0.310f, 1.00f },

		{ 0.38f, 0.58f, 0.72f, 1.00f },
		{ 0.30f, 0.45f, 0.58f, 1.00f },
		{ 0.46f, 0.70f, 0.88f, 1.00f },

		{ 0.88f, 0.90f, 0.92f, 1.00f },
		{ 0.55f, 0.58f, 0.62f, 1.00f },

		{ 0.05f, 0.06f, 0.07f, 1.00f },
		{ 0.11f, 0.13f, 0.15f, 1.00f },

		{ 0.30f, 0.50f, 0.65f, 0.40f },
		{ 0.80f, 0.22f, 0.22f, 1.00f }
	};
}

inline EditorPalette GetAmberTerminalEditorPalette() {
	return {
		"Amber",
		{ 0.060f, 0.050f, 0.030f, 1.00f },
		{ 0.090f, 0.075f, 0.045f, 1.00f },
		{ 0.130f, 0.110f, 0.070f, 1.00f },

		{ 0.240f, 0.200f, 0.120f, 1.00f },
		{ 0.320f, 0.270f, 0.170f, 1.00f },

		{ 0.90f, 0.60f, 0.20f, 1.00f },
		{ 0.65f, 0.45f, 0.18f, 1.00f },
		{ 1.00f, 0.72f, 0.30f, 1.00f },

		{ 0.96f, 0.88f, 0.70f, 1.00f },
		{ 0.65f, 0.55f, 0.38f, 1.00f },

		{ 0.04f, 0.03f, 0.02f, 1.00f },
		{ 0.10f, 0.08f, 0.05f, 1.00f },

		{ 0.90f, 0.60f, 0.20f, 0.35f },
		{ 0.85f, 0.30f, 0.20f, 1.00f }
	};
}

inline EditorPalette GetCrimsonSignalEditorPalette() {
	return {
		"Crimson Signal",
		{ 0.090f, 0.060f, 0.060f, 1.00f },
		{ 0.120f, 0.080f, 0.080f, 1.00f },
		{ 0.160f, 0.105f, 0.105f, 1.00f },

		{ 0.260f, 0.160f, 0.160f, 1.00f },
		{ 0.340f, 0.200f, 0.200f, 1.00f },

		{ 0.85f, 0.22f, 0.25f, 1.00f },
		{ 0.60f, 0.18f, 0.20f, 1.00f },
		{ 0.95f, 0.30f, 0.32f, 1.00f },

		{ 0.92f, 0.88f, 0.88f, 1.00f },
		{ 0.60f, 0.50f, 0.50f, 1.00f },

		{ 0.07f, 0.04f, 0.04f, 1.00f },
		{ 0.14f, 0.08f, 0.08f, 1.00f },

		{ 0.85f, 0.22f, 0.25f, 0.35f },
		{ 0.95f, 0.30f, 0.32f, 1.00f }
	};
}

inline std::vector<EditorPalette> GetAllEditorPalettes() {
	return {
		GetDefaultEditorPalette(),
		GetGraphiteEditorPalette(),
		GetNordSteelEditorPalette(),
		GetAmberTerminalEditorPalette(),
		GetCrimsonSignalEditorPalette()
	};
}

}
