// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "movement_system.hpp"

#include <lge/components/placement.hpp>
#include <lge/core/result.hpp>

#include "movement.hpp"

namespace examples {

auto movement_system::update(const float dt) -> lge::result<> {
	for(const auto view = ctx.world.view<lge::placement, movement>(); const auto entity: view) {
		auto &mov = view.get<movement>(entity);
		if(mov.delay > 0.0F) {
			mov.delay -= dt;
			continue;
		}
		auto &plc = view.get<lge::placement>(entity);
		plc.position += mov.velocity * dt;
		plc.rotation += mov.rotation_speed * dt;
	}
	return true;
}
} // namespace examples