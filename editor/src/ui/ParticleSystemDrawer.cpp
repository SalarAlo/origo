
#include <glm/vec3.hpp>

#include "origo/components/particle_system/ParticleEmissionShapeDefault.h"
#include "origo/components/particle_system/ParticleEmissionShapeKindVisitor.h"
#include "origo/components/particle_system/ParticleSystemComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::RegisterNativeDrawer<Origo::ParticleSystemComponent>(
	    "Particle System",
	    "./icons/ParticleSystem.svg",
	    [](Origo::ParticleSystemComponent& ps) {
		    bool generalOpen = ComponentUI::StartRegion("General");

		    if (generalOpen) {
			    ComponentUI::DrawBoolControl("Is Looping", ps.IsLooping);
			    ComponentUI::DrawFloatControl("Spawn Rate", ps.SpawnRate);

			    static bool useSizeRange = true;
			    ComponentUI::DrawBoolControl("Use Size Range", useSizeRange);

			    if (useSizeRange) {
				    glm::vec2 sizeRange { ps.StartSize, ps.EndSize };
				    ComponentUI::DrawVec2Control("Size Range", sizeRange);
				    ps.StartSize = sizeRange.x;
				    ps.EndSize = sizeRange.y;
			    } else {
				    float size = ps.StartSize;
				    ComponentUI::DrawFloatControl("Size", size);
				    ps.StartSize = size;
				    ps.EndSize = size;
			    }

			    static bool useLifetimeRange = true;
			    ComponentUI::DrawBoolControl("Use Lifetime Range", useLifetimeRange);

			    if (useLifetimeRange) {
				    glm::vec2 lifetimeRange { ps.LifetimeMin, ps.LifetimeMax };
				    ComponentUI::DrawMinMaxRangeControl("Lifetime Range", lifetimeRange);
				    ps.LifetimeMin = lifetimeRange.x;
				    ps.LifetimeMax = lifetimeRange.y;
			    } else {
				    float lifetime = ps.LifetimeMin;
				    ComponentUI::DrawFloatControl("Lifetime", lifetime);
				    ps.LifetimeMin = lifetime;
				    ps.LifetimeMax = lifetime;
			    }

			    static bool useSpeedRange = true;
			    ComponentUI::DrawBoolControl("Use Speed Range", useSpeedRange);

			    if (useSpeedRange) {
				    glm::vec2 speedRange { ps.InitialSpeedMin, ps.InitialSpeedMax };
				    ComponentUI::DrawMinMaxRangeControl("Speed Range", speedRange);
				    ps.InitialSpeedMin = speedRange.x;
				    ps.InitialSpeedMax = speedRange.y;
			    } else {
				    float speed = ps.InitialSpeedMin;
				    ComponentUI::DrawFloatControl("Speed", speed);
				    ps.InitialSpeedMin = speed;
				    ps.InitialSpeedMax = speed;
			    }
		    }

		    ComponentUI::EndRegion(generalOpen);

		    bool emissionOpen = ComponentUI::StartRegion("Emission");

		    if (emissionOpen) {
			    auto kind = std::visit(Origo::ParticleEmissionShapeKindVisitor {}, ps.Shape);
			    ComponentUI::DrawEnumControl("Shape", kind);
			    ps.Shape = Origo::ParticleEmissionShapeDefault::DefaultShape(kind);
		    }

		    ComponentUI::EndRegion(emissionOpen);

		    bool physicsOpen = ComponentUI::StartRegion("Physics");

		    if (physicsOpen) {
			    ComponentUI::DrawBoolControl("Use Gravity", ps.UseGravity);

			    if (ps.UseGravity || ps.Drag > 0.0f || ps.Mass != 1.0f) {
				    ComponentUI::DrawFloatControl("Mass", ps.Mass);
				    ComponentUI::DrawFloatControl("Drag", ps.Drag);
			    }
		    }

		    ComponentUI::EndRegion(physicsOpen);

		    bool appearanceOpen = ComponentUI::StartRegion("Appearance");

		    if (appearanceOpen) {
			    ComponentUI::DrawAssetControl("Particle Mesh", ps.ParticleMesh, Origo::AssetType::Mesh);
		    }

		    ComponentUI::EndRegion(appearanceOpen);
	    });
	return true;
}();

}
