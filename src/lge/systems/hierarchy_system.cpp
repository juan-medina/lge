// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/hierarchy.hpp>
#include <lge/components/position.hpp>
#include <lge/result.hpp>
#include <lge/systems/hierarchy_system.hpp>
#include <lge/systems/system.hpp>

#include <entity/fwd.hpp>

namespace lge {

hierarchy_system::hierarchy_system(entt::registry &world): system(world) {
	world.on_destroy<parent>().connect<&hierarchy_system::on_child_detached>();
	world.on_destroy<children>().connect<&hierarchy_system::on_parent_children_cleared>();
}

auto hierarchy_system::update(const float /*dt*/) -> result<> {
	for(const auto roots = world.view<local_position>(entt::exclude<parent>); auto e: roots) {
		resolve_node(e, glm::vec2{0, 0});
	}

	return true;
}
void hierarchy_system::resolve_node(const entt::entity entity, const glm::vec2 parent_pos) { // NOLINT(*-no-recursion)
	const auto &local = world.get<local_position>(entity);
	const glm::vec2 global = parent_pos + local.value;

	world.emplace_or_replace<global_position>(entity, global);

	if(world.all_of<children>(entity)) {
		for(const auto &kids = world.get<children>(entity).ids; const auto child: kids) {
			resolve_node(child, global);
		}
	}
}

auto hierarchy_system::on_child_detached(entt::registry &world, const entt::entity child) -> void {
	if(const entt::entity p = world.get<parent>(child).id; world.all_of<children>(p)) {
		auto &kids = world.get<children>(p).ids;
		std::erase(kids, child);
	}
}

auto hierarchy_system::on_parent_children_cleared(entt::registry &world, const entt::entity parent) -> void {
	for(const auto kids = world.get<children>(parent).ids; const entt::entity child: kids) {
		world.destroy(child);
	}
}

} // namespace lge