// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "move_random_system.hpp"

#include <lge/components/placement.hpp>
#include <lge/result.hpp>

#include <entity/fwd.hpp>
#include <entt/entt.hpp>

auto move_random_system::update(const float dt) -> lge::result<> {
	for(const auto view = world.view<lge::placement, tag>(); const auto e: view) {
		auto &plc = view.get<lge::placement>(e);
		plc.rotation -= dt * 90.0F;
	}

	return true;
}
