// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/aabb.hpp>
#include <lge/components/label.hpp>
#include <lge/components/position.hpp>
#include <lge/result.hpp>
#include <lge/systems/render_system.hpp>

#include <entt/entt.hpp>

namespace lge {

auto render_system::update(const float /*dt*/) -> result<> {
	for(const auto view = world.view<global_position>(); const auto entity: view) {
		// we ignore for now
		[[maybe_unused]] const auto &pos = view.get<global_position>(entity);
		if(world.all_of<global_aabb>(entity)) {
			auto &ga = world.get<global_aabb>(entity);

			// Draw label if present
			if(world.all_of<label>(entity)) {
				const auto &lbl = world.get<label>(entity);
				renderer_.render_label(lbl.text, lbl.size, lbl.color, ga.min);
			}

			// Draw AABB if debug draw is enabled
			if(renderer_.is_debug_draw()) {
				renderer_.render_rectangle(ga.min, ga.max, {1, 0, 0, 0.5F});
			}
		}
	}

	return true;
}

} // namespace lge