#include <glm/vec3.hpp>

#include "origo/components/RigidbodyComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::RigidbodyComponent>(
	    "Rigidbody",
	    "icons/PhysicsBody2D.svg",
	    [](Origo::RigidbodyComponent& rb) {
		    const bool body_open = ComponentUI::start_region("Body");
		    if (body_open) {
			    ComponentUI::draw_enum_control("Motion Type", rb.MotionType);

			    const bool is_dynamic = rb.MotionType == Origo::RigidbodyMotionType::Dynamic;
			    const bool supports_velocity = rb.MotionType != Origo::RigidbodyMotionType::Static;

			    ImGui::BeginDisabled(!is_dynamic);
			    ComponentUI::draw_float_control("Mass", rb.Mass);
			    ImGui::EndDisabled();

			    ImGui::BeginDisabled(!supports_velocity);
			    ComponentUI::draw_float_control("Drag", rb.Drag);
			    ComponentUI::draw_float_control("Angular drag", rb.AngularDrag);
			    ImGui::EndDisabled();
		    }
		    ComponentUI::end_region(body_open);

		    const bool material_open = ComponentUI::start_region("Material");
		    if (material_open) {
			    ComponentUI::draw_float_slider_control("Friction", rb.Friction, 0.0f, 2.0f);
			    ComponentUI::draw_float_slider_control("Restitution", rb.Restitution, 0.0f, 1.0f);
			    ComponentUI::draw_bool_control("Is sensor", rb.IsSensor);
		    }
		    ComponentUI::end_region(material_open);

		    const bool simulation_open = ComponentUI::start_region("Simulation");
		    if (simulation_open) {
			    const bool supports_simulation = rb.MotionType == Origo::RigidbodyMotionType::Dynamic;

			    ImGui::BeginDisabled(!supports_simulation);
			    ComponentUI::draw_bool_control("Use gravity", rb.UseGravity);
			    ComponentUI::draw_bool_control("Allow sleeping", rb.AllowSleeping);
			    ComponentUI::draw_enum_control("Motion Quality", rb.MotionQuality);
			    ImGui::EndDisabled();
		    }
		    ComponentUI::end_region(simulation_open);
	    });

	return true;
}();

}
