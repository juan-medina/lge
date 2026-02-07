// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/dirty.hpp>
#include <lge/components/hierarchy.hpp>
#include <lge/log.hpp>
#include <lge/result.hpp>
#include <lge/systems/dirty_propagation_system.hpp>

#include <entity/fwd.hpp>

namespace lge {

auto dirty_propagation_system::update(const float /*dt*/) -> result<> {
	for(const auto roots = world.view<dirty>(entt::exclude<parent>); auto e: roots) {
		LGE_DEBUG("entity {} is dirty, propagating to children", static_cast<std::uint32_t>(e));
		resolve_node(e);
	}

	return true;
}

auto dirty_propagation_system::resolve_node(const entt::entity entity) -> void { // NOLINT(*-no-recursion)
	world.emplace_or_replace<dirty>(entity);
	LGE_DEBUG("marked entity {} as dirty", static_cast<std::uint32_t>(entity));
	if(world.all_of<children>(entity)) {
		for(const auto &kids = world.get<children>(entity).ids; const auto child: kids) {
			LGE_DEBUG("propagating dirty state from entity {} to child {}",
					  static_cast<std::uint32_t>(entity),
					  static_cast<std::uint32_t>(child));
			resolve_node(child);
		}
	}
}

} // namespace lge