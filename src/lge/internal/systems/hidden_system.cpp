// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "hidden_system.hpp"

#include <lge/components/hidden.hpp>
#include <lge/components/hierarchy.hpp>
#include <lge/internal/components/effective_hidden.hpp>
#include <lge/result.hpp>

#include <entity/fwd.hpp>
#include <entt/entt.hpp>
#include <vector>

namespace lge {

auto hidden_system::update(const float /*dt*/) -> result<> {
	for(const auto entity: world.view<entt::entity>(entt::exclude<parent>)) {
		if(world.any_of<hidden>(entity)) {
			world.emplace_or_replace<effective_hidden>(entity);
		} else {
			world.remove<effective_hidden>(entity);
		}
		hidden_stack_.push_back(entity);
	}

	while(!hidden_stack_.empty()) {
		const auto entity = hidden_stack_.back();
		hidden_stack_.pop_back();

		// if the parent is hidden, all children are hidden, otherwise we need to check each child
		const bool parent_hidden = world.any_of<effective_hidden>(entity);
		if(world.any_of<children>(entity)) {
			if(parent_hidden) {
				for(auto child: world.get<children>(entity).ids) {
					world.emplace_or_replace<effective_hidden>(child);
					hidden_stack_.push_back(child);
				}
			} else {
				for(auto child: world.get<children>(entity).ids) {
					if(world.any_of<hidden>(child)) {
						world.emplace_or_replace<effective_hidden>(child);
					} else {
						world.remove<effective_hidden>(child);
					}
					hidden_stack_.push_back(child);
				}
			}
		}
	}

	return true;
}

} // namespace lge