// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "order_system.hpp"

#include <lge/components/hierarchy.hpp>
#include <lge/components/order.hpp>
#include <lge/internal/components/render_order.hpp>
#include <lge/result.hpp>

#include <entity/fwd.hpp>
#include <entt/entt.hpp>

namespace lge {

auto order_system::update(const float /*dt*/) -> result<> {
	for(const auto entity: world.view<order>(entt::exclude<parent>)) {
		const auto &local = world.get<order>(entity);
		world.emplace_or_replace<render_order>(entity, render_order{.layer = local.layer, .index = local.index});
		order_stack_.push_back(entity);
	}

	while(!order_stack_.empty()) {
		const auto entity = order_stack_.back();
		order_stack_.pop_back();

		const auto &parent_render_order = world.get<render_order>(entity);

		if(world.any_of<children>(entity)) {
			for(const auto &kids = world.get<children>(entity).ids; const auto child: kids) {
				const auto &local = world.get_or_emplace<order>(child);
				const auto child_layer = parent_render_order.layer;
				const auto child_index = parent_render_order.index + local.index;

				world.emplace_or_replace<render_order>(child, render_order{.layer = child_layer, .index = child_index});
				order_stack_.push_back(child);
			}
		}
	}

	return true;
}

} // namespace lge