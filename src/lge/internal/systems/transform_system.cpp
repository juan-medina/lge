// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "transform_system.hpp"

#include <lge/components/hierarchy.hpp>
#include <lge/components/placement.hpp>
#include <lge/internal/components/metrics.hpp>
#include <lge/internal/components/transform.hpp>
#include <lge/result.hpp>
#include <lge/systems/system.hpp>

#include <entity/fwd.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/trigonometric.hpp>
#include <vector>

namespace lge {

transform_system::transform_system(const phase p, entt::registry &world): system(p, world) {
	world.on_destroy<parent>().connect<&transform_system::on_child_detached>();
	world.on_destroy<children>().connect<&transform_system::on_parent_children_cleared>();
}

auto transform_system::compose_transform(const placement &node_placement, const glm::vec2 &pivot_offset) -> glm::mat3 {
	const float rad = glm::radians(node_placement.rotation);
	const float s = glm::sin(rad);
	const float c = glm::cos(rad);

	const auto rs = glm::mat3{{c * node_placement.scale.x, -s * node_placement.scale.y, 0.F},
							  {s * node_placement.scale.x, c * node_placement.scale.y, 0.F},
							  {0.F, 0.F, 1.F}};

	const auto to_pivot = glm::mat3{{1.F, 0.F, 0.F}, {0.F, 1.F, 0.F}, {-pivot_offset.x, -pivot_offset.y, 1.F}};

	const auto from_pivot = glm::mat3{{1.F, 0.F, 0.F}, {0.F, 1.F, 0.F}, {pivot_offset.x, pivot_offset.y, 1.F}};

	const auto translation =
		glm::mat3{{1.F, 0.F, 0.F}, {0.F, 1.F, 0.F}, {node_placement.position.x, node_placement.position.y, 1.F}};

	return translation * from_pivot * rs * to_pivot;
}

auto transform_system::update(const float /*dt*/) -> result<> {
	for(const auto entity: world.view<placement>(entt::exclude<parent>)) {
		const auto &local = world.get<placement>(entity);
		const auto pivot_offset =
			world.all_of<metrics>(entity) ? local.pivot * world.get<metrics>(entity).size : glm::vec2{0.F, 0.F};
		const auto world_mat = compose_transform(local, pivot_offset);
		world.emplace_or_replace<transform>(entity, transform{.world = world_mat});
		transform_stack_.push_back(entity);
	}

	while(!transform_stack_.empty()) {
		const auto entity = transform_stack_.back();
		transform_stack_.pop_back();

		const auto &[node_transform] = world.get<transform>(entity);

		if(world.any_of<children>(entity)) {
			for(const auto &kids = world.get<children>(entity).ids; const auto child: kids) {
				const auto &local = world.get<placement>(child);
				const auto pivot_offset =
					world.all_of<metrics>(child) ? local.pivot * world.get<metrics>(child).size : glm::vec2{0.F, 0.F};
				const auto local_mat = compose_transform(local, pivot_offset);
				const auto world_mat = node_transform * local_mat;

				world.emplace_or_replace<transform>(child, transform{.world = world_mat});
				transform_stack_.push_back(child);
			}
		}
	}

	return true;
}

auto transform_system::on_child_detached(entt::registry &world, const entt::entity child) -> void {
	if(const entt::entity p = world.get<parent>(child).id; world.all_of<children>(p)) {
		auto &kids = world.get<children>(p).ids;
		std::erase(kids, child);
	}
}

auto transform_system::on_parent_children_cleared(entt::registry &world, const entt::entity parent) -> void {
	for(const auto kids = world.get<children>(parent).ids; const entt::entity child: kids) {
		world.destroy(child);
	}
}

} // namespace lge