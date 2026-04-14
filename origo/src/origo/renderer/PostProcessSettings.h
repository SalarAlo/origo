#pragma once

namespace Origo {

enum class ToneMappingOperator {
	Reinhard,
	ACES,
	Neutral
};

struct ToneMappingSettings {
	bool Enabled { true };
	ToneMappingOperator Operator { ToneMappingOperator::Reinhard };
	float Exposure { 0.0f };
	float Gamma { 2.2f };
};

struct ColorAdjustmentsSettings {
	bool Enabled { false };
	float Contrast { 0.0f };
	float Saturation { 1.0f };
	float HueShift { 0.0f };
	float Temperature { 0.0f };
	float Tint { 0.0f };
};

struct BloomSettings {
	bool Enabled { false };
	float Threshold { 1.0f };
	float Knee { 0.5f };
	float Intensity { 0.7f };
	int BlurPasses { 4 };
};

struct VignetteSettings {
	bool Enabled { false };
	float Intensity { 0.25f };
	float Smoothness { 0.55f };
	float Roundness { 1.0f };
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
