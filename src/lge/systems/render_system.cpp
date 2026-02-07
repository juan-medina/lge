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
		const auto &pos = view.get<global_position>(entity);

		// Draw label if present
		if(world.all_of<label>(entity)) {
			const auto &lbl = world.get<label>(entity);
			renderer_.render_label(lbl, pos.value);
		}

		// Draw AABB if present and debug draw is enabled
		if(renderer_.is_debug_draw() && world.all_of<global_aabb>(entity)) {
			const auto &ga = world.get<global_aabb>(entity);
			renderer_.render_aabb(ga);
		}
	}

	return true;
}
} // namespace lge