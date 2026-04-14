#include "CameraComponentSerializer.h"

#include "origo/components/CameraComponent.h"

namespace Origo {

void CameraComponentSerializer::serialize(Component* comp, ISerializer& backend) const {
	auto camera_component { static_cast<CameraComponent*>(comp) };
	auto& camera { camera_component->CameraObj };
	auto& post = camera_component->PostProcess;

	backend.write("near", camera.get_near());
	backend.write("far", camera.get_far());
	backend.write("fov", camera.FOV);
	backend.write("is_primary", camera_component->IsPrimary ? 1 : 0);

	backend.begin_object("post_processing");
	backend.write("enabled", post.Enabled);

	backend.begin_object("tone_mapping");
	backend.write("enabled", post.ToneMapping.Enabled);
	backend.write("operator", static_cast<int>(post.ToneMapping.Operator));
	backend.write("exposure", post.ToneMapping.Exposure);
	backend.write("gamma", post.ToneMapping.Gamma);
	backend.end_object();

	backend.begin_object("color_adjustments");
	backend.write("enabled", post.ColorAdjustments.Enabled);
	backend.write("contrast", post.ColorAdjustments.Contrast);
	backend.write("saturation", post.ColorAdjustments.Saturation);
	backend.write("hue_shift", post.ColorAdjustments.HueShift);
	backend.write("temperature", post.ColorAdjustments.Temperature);
	backend.write("tint", post.ColorAdjustments.Tint);
	backend.end_object();

	backend.begin_object("bloom");
	backend.write("enabled", post.Bloom.Enabled);
	backend.write("threshold", post.Bloom.Threshold);
	backend.write("knee", post.Bloom.Knee);
	backend.write("intensity", post.Bloom.Intensity);
	backend.write("blur_passes", post.Bloom.BlurPasses);
	backend.end_object();

	backend.begin_object("vignette");
	backend.write("enabled", post.Vignette.Enabled);
	backend.write("intensity", post.Vignette.Intensity);
	backend.write("smoothness", post.Vignette.Smoothness);
	backend.write("roundness", post.Vignette.Roundness);
	backend.write("color", post.Vignette.Color);
	backend.end_object();

	backend.begin_object("chromatic_aberration");
	backend.write("enabled", post.ChromaticAberration.Enabled);
	backend.write("intensity", post.ChromaticAberration.Intensity);
	backend.end_object();

	backend.begin_object("film_grain");
	backend.write("enabled", post.FilmGrain.Enabled);
	backend.write("intensity", post.FilmGrain.Intensity);
	backend.write("response", post.FilmGrain.Response);
	backend.end_object();
	backend.end_object();
}

void CameraComponentSerializer::deserialize(Component* comp, ISerializer& backend) {
	auto camera_component { static_cast<CameraComponent*>(comp) };
	auto& camera { camera_component->CameraObj };
	auto& post = camera_component->PostProcess;

	int is_primary_int {};
	float near {};
	float far {};

	backend.try_read("near", near);
	backend.try_read("far", far);
	backend.try_read("fov", camera.FOV);
	backend.try_read("is_primary", is_primary_int);

	camera_component->IsPrimary = is_primary_int != 0;
	camera_component->CameraObj.set_near(near);
	camera_component->CameraObj.set_far(far);

	backend.begin_object("post_processing");
	backend.try_read("enabled", post.Enabled);

	backend.begin_object("tone_mapping");
	int operator_int = static_cast<int>(post.ToneMapping.Operator);
	backend.try_read("enabled", post.ToneMapping.Enabled);
	backend.try_read("operator", operator_int);
	post.ToneMapping.Operator = static_cast<ToneMappingOperator>(operator_int);
	backend.try_read("exposure", post.ToneMapping.Exposure);
	backend.try_read("gamma", post.ToneMapping.Gamma);
	backend.end_object();

	backend.begin_object("color_adjustments");
	backend.try_read("enabled", post.ColorAdjustments.Enabled);
	backend.try_read("contrast", post.ColorAdjustments.Contrast);
	backend.try_read("saturation", post.ColorAdjustments.Saturation);
	backend.try_read("hue_shift", post.ColorAdjustments.HueShift);
	backend.try_read("temperature", post.ColorAdjustments.Temperature);
	backend.try_read("tint", post.ColorAdjustments.Tint);
	backend.end_object();

	backend.begin_object("bloom");
	backend.try_read("enabled", post.Bloom.Enabled);
	backend.try_read("threshold", post.Bloom.Threshold);
	backend.try_read("knee", post.Bloom.Knee);
	backend.try_read("intensity", post.Bloom.Intensity);
	backend.try_read("blur_passes", post.Bloom.BlurPasses);
	backend.end_object();

	backend.begin_object("vignette");
	backend.try_read("enabled", post.Vignette.Enabled);
	backend.try_read("intensity", post.Vignette.Intensity);
	backend.try_read("smoothness", post.Vignette.Smoothness);
	backend.try_read("roundness", post.Vignette.Roundness);
	backend.try_read("color", post.Vignette.Color);
	backend.end_object();

	backend.begin_object("chromatic_aberration");
	backend.try_read("enabled", post.ChromaticAberration.Enabled);
	backend.try_read("intensity", post.ChromaticAberration.Intensity);
	backend.end_object();

	backend.begin_object("film_grain");
	backend.try_read("enabled", post.FilmGrain.Enabled);
	backend.try_read("intensity", post.FilmGrain.Intensity);
	backend.try_read("response", post.FilmGrain.Response);
	backend.end_object();
	backend.end_object();
};

}
