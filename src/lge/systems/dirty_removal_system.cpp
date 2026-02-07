// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/dirty.hpp>
#include <lge/result.hpp>
#include <lge/systems/dirty_removal_system.hpp>

#include <entt/entt.hpp>

namespace lge {

auto dirty_removal_system::update(const float /*dt*/) -> result<> {
	for(const auto roots = world.view<dirty>(); auto e: roots) {
		world.remove<dirty>(e);
	}

	return true;
}

} // namespace lge