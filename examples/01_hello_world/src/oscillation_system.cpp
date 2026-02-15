// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "oscillation_system.hpp"

#include <lge/components/hidden.hpp>
#include <lge/components/placement.hpp>
#include <lge/core/result.hpp>
#include <lge/effects/oscillator.hpp>

#include <entity/fwd.hpp>
#include <entt/entt.hpp>

auto oscillation_system::update(const float dt) -> lge::result<> {
	for(const auto view = world.view<lge::placement, effect>(entt::exclude<lge::hidden>); const auto e: view) {
		auto &plc = view.get<lge::placement>(e);
		auto &[osc_scale, osc_rotation] = view.get<effect>(e);
		plc.rotation = lge::effects::update_oscillator(osc_rotation, dt);
		const auto scale = lge::effects::update_oscillator(osc_scale, dt);
		plc.scale = {scale, scale};
	}

	return true;
}
