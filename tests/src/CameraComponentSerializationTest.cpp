#include <doctest/doctest.h>

#include "origo/components/CameraComponent.h"
#include "origo/components/serialization/CameraComponentSerializer.h"
#include "origo/serialization/JsonSerializer.h"

TEST_CASE("CameraComponentSerializer roundtrip preserves post processing settings") {
	Origo::CameraComponent original;
	original.IsPrimary = true;
	original.CameraObj.FOV = 72.0f;
	original.CameraObj.set_near(0.25f);
	original.CameraObj.set_far(750.0f);

	original.PostProcess.Enabled = true;
	original.PostProcess.ToneMapping.Operator = Origo::ToneMappingOperator::ACES;
	original.PostProcess.ToneMapping.Exposure = 1.25f;
	original.PostProcess.Bloom.Enabled = true;
	original.PostProcess.Bloom.Intensity = 1.8f;
	original.PostProcess.Bloom.BlurPasses = 7;
	original.PostProcess.ColorAdjustments.Enabled = true;
	original.PostProcess.ColorAdjustments.Saturation = 1.35f;
	original.PostProcess.Vignette.Enabled = true;
	original.PostProcess.Vignette.Color = { 0.1f, 0.05f, 0.2f };
	original.PostProcess.ChromaticAberration.Enabled = true;
	original.PostProcess.ChromaticAberration.Intensity = 0.004f;
	original.PostProcess.FilmGrain.Enabled = true;
	original.PostProcess.FilmGrain.Response = 0.85f;

	Origo::CameraComponentSerializer serializer;
	Origo::JsonSerializer json;

	serializer.serialize(&original, json);

	Origo::CameraComponent restored;
	serializer.deserialize(&restored, json);

	CHECK(restored.IsPrimary == true);
	CHECK(restored.CameraObj.FOV == doctest::Approx(72.0f));
	CHECK(restored.CameraObj.get_near() == doctest::Approx(0.25f));
	CHECK(restored.CameraObj.get_far() == doctest::Approx(750.0f));

	CHECK(restored.PostProcess.Enabled == true);
	CHECK(restored.PostProcess.ToneMapping.Operator == Origo::ToneMappingOperator::ACES);
	CHECK(restored.PostProcess.ToneMapping.Exposure == doctest::Approx(1.25f));
	CHECK(restored.PostProcess.Bloom.Enabled == true);
	CHECK(restored.PostProcess.Bloom.Intensity == doctest::Approx(1.8f));
	CHECK(restored.PostProcess.Bloom.BlurPasses == 7);
	CHECK(restored.PostProcess.ColorAdjustments.Enabled == true);
	CHECK(restored.PostProcess.ColorAdjustments.Saturation == doctest::Approx(1.35f));
	CHECK(restored.PostProcess.Vignette.Enabled == true);
	CHECK(restored.PostProcess.Vignette.Color.x == doctest::Approx(0.1f));
	CHECK(restored.PostProcess.Vignette.Color.y == doctest::Approx(0.05f));
	CHECK(restored.PostProcess.Vignette.Color.z == doctest::Approx(0.2f));
	CHECK(restored.PostProcess.ChromaticAberration.Enabled == true);
	CHECK(restored.PostProcess.ChromaticAberration.Intensity == doctest::Approx(0.004f));
	CHECK(restored.PostProcess.FilmGrain.Enabled == true);
	CHECK(restored.PostProcess.FilmGrain.Response == doctest::Approx(0.85f));
}
