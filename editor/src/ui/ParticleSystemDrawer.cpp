
#include <glm/vec3.hpp>

#include "components/DrawEmissionShapeGUIControl.h"

#include "origo/components/particle_system/ParticleEmissionShapeFactory.h"
#include "origo/components/particle_system/ParticleEmissionShapeKindOf.h"
#include "origo/components/particle_system/ParticleSystemComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::register_native_drawer<Origo::ParticleSystemComponent>(
	    "Particle System",
	    "./icons/ParticleSystem.svg",
	    [](Origo::ParticleSystemComponent& ps) {
		    bool general_open = ComponentUI::start_region("General");

		    if (general_open) {
			    ComponentUI::draw_bool_control("Is Looping", ps.IsLooping);
			    ComponentUI::draw_float_control("Spawn Rate", ps.SpawnRate);

			    static bool use_size_range = true;
			    ComponentUI::draw_bool_control("Use Size Range", use_size_range);

			    if (use_size_range) {
				    glm::vec2 size_range { ps.StartSize, ps.EndSize };
				    ComponentUI::draw_vec2_control("Size Range", size_range);
				    ps.StartSize = size_range.x;
				    ps.EndSize = size_range.y;
			    } else {
				    float size = ps.StartSize;
				    ComponentUI::draw_float_control("Size", size);
				    ps.StartSize = size;
				    ps.EndSize = size;
			    }

			    static bool use_lifetime_range = true;
			    ComponentUI::draw_bool_control("Use Lifetime Range", use_lifetime_range);

			    if (use_lifetime_range) {
				    glm::vec2 lifetime_range { ps.LifetimeMin, ps.LifetimeMax };
				    ComponentUI::draw_min_max_range_control("Lifetime Range", lifetime_range);
				    ps.LifetimeMin = lifetime_range.x;
				    ps.LifetimeMax = lifetime_range.y;
			    } else {
				    float lifetime = ps.LifetimeMin;
				    ComponentUI::draw_float_control("Lifetime", lifetime);
				    ps.LifetimeMin = lifetime;
				    ps.LifetimeMax = lifetime;
			    }

			    static bool use_speed_range = true;
			    ComponentUI::draw_bool_control("Use Speed Range", use_speed_range);

			    if (use_speed_range) {
				    glm::vec2 speed_range { ps.InitialSpeedMin, ps.InitialSpeedMax };
				    ComponentUI::draw_min_max_range_control("Speed Range", speed_range);
				    ps.InitialSpeedMin = speed_range.x;
				    ps.InitialSpeedMax = speed_range.y;
			    } else {
				    float speed = ps.InitialSpeedMin;
				    ComponentUI::draw_float_control("Speed", speed);
				    ps.InitialSpeedMin = speed;
				    ps.InitialSpeedMax = speed;
			    }
		    }

		    ComponentUI::end_region(general_open);

		    bool emission_open = ComponentUI::start_region("Emission");

		    if (emission_open) {
			    auto new_kind = std::visit(Origo::ParticleEmissionShapeKindOf {}, ps.Shape);
			    auto orignal_kind = std::visit(Origo::ParticleEmissionShapeKindOf {}, ps.Shape);
			    ComponentUI::draw_enum_control("Shape", new_kind);

			    if (orignal_kind != new_kind) {
				    ps.Shape = Origo::ParticleEmissionShapeFactory::create_default(new_kind);
			    }

			    std::visit(OrigoEditor::DrawEmissionShapeGUIControls {}, ps.Shape);
		    }

		    ComponentUI::end_region(emission_open);

		    bool physics_open = ComponentUI::start_region("Physics");

		    if (physics_open) {
			    ComponentUI::draw_bool_control("Use Gravity", ps.UseGravity);

			    if (ps.UseGravity) {
				    ComponentUI::draw_float_control("Gravity Force Factor", ps.GravityForceFactor);
				    ComponentUI::draw_float_control("Drag", ps.Drag);
			    } else {
				    ps.Drag = .0f;
				    ps.GravityForceFactor = 1.0f;
			    }
		    }

		    ComponentUI::end_region(physics_open);

		    bool appearance_open = ComponentUI::start_region("Appearance");

		    if (appearance_open) {
			    ComponentUI::draw_asset_control("Particle Mesh", ps.ParticleMesh, Origo::AssetType::Mesh);
		    }

		    ComponentUI::end_region(appearance_open);
	    });
	return true;
}();

}
