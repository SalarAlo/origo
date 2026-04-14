#include <glm/vec3.hpp>

#include "origo/components/CameraComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::CameraComponent>("Camera", "./icons/Camera.svg", [](Origo::CameraComponent& cam) {
		ComponentUI::draw_float_control("FOV", cam.CameraObj.FOV);
		ComponentUI::draw_bool_control("Is Primary", cam.IsPrimary);

		const bool post_open = ComponentUI::start_region("Post Processing");
		if (post_open) {
			ComponentUI::draw_bool_control("Enabled", cam.PostProcess.Enabled);

			const bool tone_open = ComponentUI::start_region("Tone Mapping");
			if (tone_open) {
				ComponentUI::draw_bool_control("Enabled", cam.PostProcess.ToneMapping.Enabled);
				ComponentUI::draw_enum_control("Operator", cam.PostProcess.ToneMapping.Operator);
				ComponentUI::draw_float_slider_control("Exposure", cam.PostProcess.ToneMapping.Exposure, -5.0f, 5.0f, "%.2f");
				ComponentUI::draw_float_slider_control("Gamma", cam.PostProcess.ToneMapping.Gamma, 1.0f, 3.0f, "%.2f");
			}
			ComponentUI::end_region(tone_open);

			const bool color_open = ComponentUI::start_region("Color");
			if (color_open) {
				ComponentUI::draw_bool_control("Enabled", cam.PostProcess.ColorAdjustments.Enabled);
				ComponentUI::draw_float_slider_control("Contrast", cam.PostProcess.ColorAdjustments.Contrast, -1.0f, 1.0f, "%.2f");
				ComponentUI::draw_float_slider_control("Saturation", cam.PostProcess.ColorAdjustments.Saturation, 0.0f, 2.0f, "%.2f");
				ComponentUI::draw_float_slider_control("Hue Shift", cam.PostProcess.ColorAdjustments.HueShift, -180.0f, 180.0f, "%.0f");
				ComponentUI::draw_float_slider_control("Temperature", cam.PostProcess.ColorAdjustments.Temperature, -1.0f, 1.0f, "%.2f");
				ComponentUI::draw_float_slider_control("Tint", cam.PostProcess.ColorAdjustments.Tint, -1.0f, 1.0f, "%.2f");
			}
			ComponentUI::end_region(color_open);

			const bool bloom_open = ComponentUI::start_region("Bloom");
			if (bloom_open) {
				ComponentUI::draw_bool_control("Enabled", cam.PostProcess.Bloom.Enabled);
				ComponentUI::draw_float_slider_control("Threshold", cam.PostProcess.Bloom.Threshold, 0.0f, 4.0f, "%.2f");
				ComponentUI::draw_float_slider_control("Knee", cam.PostProcess.Bloom.Knee, 0.01f, 2.0f, "%.2f");
				ComponentUI::draw_float_slider_control("Intensity", cam.PostProcess.Bloom.Intensity, 0.0f, 3.0f, "%.2f");
				ComponentUI::draw_int_control("Blur Passes", cam.PostProcess.Bloom.BlurPasses, 0.1f);
				cam.PostProcess.Bloom.BlurPasses = std::clamp(cam.PostProcess.Bloom.BlurPasses, 1, 12);
			}
			ComponentUI::end_region(bloom_open);

			const bool vignette_open = ComponentUI::start_region("Vignette");
			if (vignette_open) {
				ComponentUI::draw_bool_control("Enabled", cam.PostProcess.Vignette.Enabled);
				ComponentUI::draw_float_slider_control("Intensity", cam.PostProcess.Vignette.Intensity, 0.0f, 1.0f, "%.2f");
				ComponentUI::draw_float_slider_control("Smoothness", cam.PostProcess.Vignette.Smoothness, 0.05f, 1.0f, "%.2f");
				ComponentUI::draw_float_slider_control("Roundness", cam.PostProcess.Vignette.Roundness, 0.0f, 2.0f, "%.2f");
				ComponentUI::draw_color_control("Color", cam.PostProcess.Vignette.Color);
			}
			ComponentUI::end_region(vignette_open);

			const bool chromatic_open = ComponentUI::start_region("Chromatic Aberration");
			if (chromatic_open) {
				ComponentUI::draw_bool_control("Enabled", cam.PostProcess.ChromaticAberration.Enabled);
				ComponentUI::draw_float_slider_control("Intensity", cam.PostProcess.ChromaticAberration.Intensity, 0.0f, 0.02f, "%.4f");
			}
			ComponentUI::end_region(chromatic_open);

			const bool grain_open = ComponentUI::start_region("Film Grain");
			if (grain_open) {
				ComponentUI::draw_bool_control("Enabled", cam.PostProcess.FilmGrain.Enabled);
				ComponentUI::draw_float_slider_control("Intensity", cam.PostProcess.FilmGrain.Intensity, 0.0f, 0.2f, "%.3f");
				ComponentUI::draw_float_slider_control("Response", cam.PostProcess.FilmGrain.Response, 0.0f, 1.0f, "%.2f");
			}
			ComponentUI::end_region(grain_open);
		}
		ComponentUI::end_region(post_open);
	});

	return true;
}();

}
