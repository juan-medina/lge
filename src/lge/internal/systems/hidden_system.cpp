// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "hidden_system.hpp"

#include <lge/components/hidden.hpp>
#include <lge/components/hierarchy.hpp>
#include <lge/core/result.hpp>
#include <lge/internal/components/effective_hidden.hpp>

#include <entity/fwd.hpp>
#include <entt/entt.hpp>
#include <vector>

namespace lge {

auto hidden_system::update(const float /*dt*/) -> result<> {
	for(const auto entity: ctx.world.view<entt::entity>(entt::exclude<parent>)) {
		if(ctx.world.any_of<hidden>(entity)) {
			ctx.world.emplace_or_replace<effective_hidden>(entity);
		} else {
			ctx.world.remove<effective_hidden>(entity);
		}
		hidden_stack_.push_back(entity);
	}

	while(!hidden_stack_.empty()) {
		const auto entity = hidden_stack_.back();
		hidden_stack_.pop_back();

		// if the parent is hidden, all children are hidden, otherwise we need to check each child
		const bool parent_hidden = ctx.world.any_of<effective_hidden>(entity);
		if(ctx.world.any_of<children>(entity)) {
			if(parent_hidden) {
				for(auto child: ctx.world.get<children>(entity).ids) {
					ctx.world.emplace_or_replace<effective_hidden>(child);
					hidden_stack_.push_back(child);
				}
			} else {
				for(auto child: ctx.world.get<children>(entity).ids) {
					if(ctx.world.any_of<hidden>(child)) {
						ctx.world.emplace_or_replace<effective_hidden>(child);
					} else {
						ctx.world.remove<effective_hidden>(child);
					}
					hidden_stack_.push_back(child);
				}
			}
		}
	}

	return true;
}

} // namespace lge