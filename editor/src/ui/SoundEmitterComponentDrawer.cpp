#include "origo/components/SoundEmitterComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::SoundEmitterComponent>(
	    "Sound Emitter",
	    "icons/Play.svg",
	    [](Origo::SoundEmitterComponent& emitter) {
		    ComponentUI::draw_asset_control("Audio", emitter.AudioHandle, Origo::AssetType::Audio);
		    ComponentUI::draw_bool_control("Enabled", emitter.IsEnabled);
		    ComponentUI::draw_bool_control("Auto Play", emitter.AutoPlay);
		    ComponentUI::draw_bool_control("Looping", emitter.IsLooping);
		    ComponentUI::draw_bool_control("Muted", emitter.IsMuted);
		    ComponentUI::draw_bool_control("Is Spatial", emitter.IsSpatial);

		    if (emitter.IsSpatial) {
			    ComponentUI::draw_float_control("Falloff duration", emitter.FalloffDuration);
			    ComponentUI::draw_float_control("Falloff start distance", emitter.FalloffStartDistance);
		    }

		    ComponentUI::draw_float_slider_control("Volume", emitter.Volume, 0.0f, 2.0f);
		    ComponentUI::draw_float_slider_control("Pitch", emitter.Pitch, 0.125f, 4.0f);
		    ComponentUI::draw_float_control("Start Delay", emitter.StartDelaySeconds, 0.01f);
	    });

	return true;
}();

}
