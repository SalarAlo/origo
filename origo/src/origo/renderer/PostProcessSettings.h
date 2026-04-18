#pragma once

namespace Origo {

enum class ToneMappingOperator {
	Reinhard,
	ACES,
	Neutral
};

struct ToneMappingSettings {
	bool Enabled { true };
	ToneMappingOperator Operator { ToneMappingOperator::ACES };
	float Exposure { 0.1f };
	float Gamma { 2.2f };
};

struct ColorAdjustmentsSettings {
	bool Enabled { true };
	float Contrast { 0.06f };
	float Saturation { 1.08f };
	float HueShift { 0.0f };
	float Temperature { 0.02f };
	float Tint { 0.0f };
};

struct BloomSettings {
	bool Enabled { true };
	float Threshold { 1.5f };
	float Knee { 0.45f };
	float Intensity { 0.08f };
	int BlurPasses { 4 };
};

struct VignetteSettings {
	bool Enabled { true };
	float Intensity { 0.08f };
	float Smoothness { 0.72f };
	float Roundness { 1.1f };
	Vec3 Color { 0.0f, 0.0f, 0.0f };
};

struct ChromaticAberrationSettings {
	bool Enabled { false };
	float Intensity { 0.0015f };
};

struct FilmGrainSettings {
	bool Enabled { false };
	float Intensity { 0.03f };
	float Response { 0.5f };
};

struct PostProcessSettings {
	bool Enabled { true };
	ToneMappingSettings ToneMapping {};
	ColorAdjustmentsSettings ColorAdjustments {};
	BloomSettings Bloom {};
	VignetteSettings Vignette {};
	ChromaticAberrationSettings ChromaticAberration {};
	FilmGrainSettings FilmGrain {};
};

}
