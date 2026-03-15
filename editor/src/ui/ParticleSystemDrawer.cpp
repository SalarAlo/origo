
#include <algorithm>
#include <string>

#include <glm/vec3.hpp>

#include "components/DrawEmissionShapeGUIControl.h"

#include "imgui.h"

#include "origo/components/particle_system/ParticleEmissionShapeFactory.h"
#include "origo/components/particle_system/ParticleEmissionShapeKindOf.h"
#include "origo/components/particle_system/ParticleSystemComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::ParticleSystemComponent>(
	    "Particle System",
	    "./icons/ParticleSystem.svg",
	    [](Origo::ParticleSystemComponent& ps) {
		    bool general_open = ComponentUI::start_region("General");

		    if (general_open) {
			    ComponentUI::draw_bool_control("Looping", ps.IsLooping);
			    ComponentUI::draw_float_control("Start Delay", ps.StartDelay);

			    ImGui::BeginDisabled(!ps.IsLooping);
			    ComponentUI::draw_float_control("Particles Per Second", ps.SpawnRate);
			    ImGui::EndDisabled();

			    ps.StartDelay = std::max(0.0f, ps.StartDelay);
			    ps.SpawnRate = std::max(0.0f, ps.SpawnRate);

			    ImGui::Spacing();

			    glm::vec2 size_range { ps.StartSize, ps.EndSize };
			    ComponentUI::draw_vec2_control("Size Start / End", size_range);
			    ps.StartSize = size_range.x;
			    ps.EndSize = size_range.y;

			    glm::vec2 lifetime_range { ps.LifetimeMin, ps.LifetimeMax };
			    ComponentUI::draw_min_max_range_control("Lifetime Min / Max", lifetime_range);
			    ps.LifetimeMin = lifetime_range.x;
			    ps.LifetimeMax = lifetime_range.y;

			    glm::vec2 speed_range { ps.InitialSpeedMin, ps.InitialSpeedMax };
			    ComponentUI::draw_min_max_range_control("Speed Min / Max", speed_range);
			    ps.InitialSpeedMin = speed_range.x;
			    ps.InitialSpeedMax = speed_range.y;
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

		    bool bursts_open = ComponentUI::start_region("Bursts");

		    if (bursts_open) {
			    if (ImGui::Button("Add Burst"))
				    ps.Bursts.push_back({});

			    int remove_index = -1;

			    for (size_t i = 0; i < ps.Bursts.size(); ++i) {
				    auto& burst = ps.Bursts[i];
				    burst.Time = std::max(0.0f, burst.Time);
				    burst.Count = std::max(0, burst.Count);

				    ImGui::PushID(static_cast<int>(i));

				    const std::string header = "Burst " + std::to_string(i + 1);
				    bool is_open = false;

				    if (ImGui::BeginTable("BurstRow", 2, ImGuiTableFlags_SizingStretchProp)) {
					    ImGui::TableSetupColumn("Header", ImGuiTableColumnFlags_WidthStretch);
					    ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 84.0f);
					    ImGui::TableNextRow();

					    ImGui::TableSetColumnIndex(0);
					    is_open = ImGui::CollapsingHeader(
					        header.c_str(),
					        ImGuiTreeNodeFlags_DefaultOpen);

					    ImGui::TableSetColumnIndex(1);
					    if (ImGui::Button("Remove", ImVec2(-1.0f, 0.0f)))
						    remove_index = static_cast<int>(i);

					    ImGui::EndTable();
				    }

				    if (is_open) {
					    ComponentUI::draw_float_control("Time After Start", burst.Time);
					    ComponentUI::draw_int_control("Count", burst.Count);

					    burst.Time = std::max(0.0f, burst.Time);
					    burst.Count = std::max(0, burst.Count);
				    }

				    ImGui::Spacing();
				    ImGui::PopID();
			    }

			    if (remove_index >= 0)
				    ps.Bursts.erase(ps.Bursts.begin() + remove_index);
		    }

		    ComponentUI::end_region(bursts_open);

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
			    ComponentUI::draw_asset_control("Particle Material", ps.ParticleMaterial, Origo::AssetType::MaterialPBR);
		    }

		    ComponentUI::end_region(appearance_open);
	    });
	return true;
}();

}
