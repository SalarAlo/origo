#include "origo/components/SoundEmitter.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::SoundEmitter>(
	    "Sound Emitter",
	    "icons/Play.svg",
	    [](Origo::SoundEmitter& emitter) {
		    ComponentUI::draw_asset_control("Audio", emitter.AudioHandle, Origo::AssetType::Audio);
		    ComponentUI::draw_float_control("Volume", emitter.Volume);
		    ComponentUI::draw_bool_control("Looping", emitter.IsLooping);
	    });

	return true;
}();

}
