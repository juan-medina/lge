// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "transform_system.hpp"

#include <lge/components/hierarchy.hpp>
#include <lge/components/placement.hpp>
#include <lge/core/result.hpp>
#include <lge/internal/components/metrics.hpp>
#include <lge/internal/components/transform.hpp>
#include <lge/systems/system.hpp>

#include <entity/fwd.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/trigonometric.hpp>
#include <vector>

namespace lge {

transform_system::transform_system(const phase p, entt::registry &world): system(p, world) {
	world.on_destroy<parent>().connect<&transform_system::on_child_detached>();
	world.on_destroy<children>().connect<&transform_system::on_parent_children_cleared>();
}

auto transform_system::compose_transform(const placement &node_placement, const glm::vec2 &pivot_offset) -> glm::mat3 {
	static constexpr auto x = glm::vec3{1.F, 0.F, 0.F};
	static constexpr auto y = glm::vec3{0.F, 1.F, 0.F};
	static constexpr auto z = glm::vec3{0.F, 0.F, 1.F};

	const float rad = glm::radians(node_placement.rotation);
	const float s = glm::sin(rad);
	const float c = glm::cos(rad);

	const auto rotation_scale = glm::mat3{{c * node_placement.scale.x, s * node_placement.scale.y, 0.F},
										  {-s * node_placement.scale.x, c * node_placement.scale.y, 0.F},
										  z};

	const auto to_pivot = glm::mat3{x, y, {-pivot_offset.x, -pivot_offset.y, 1.F}};
	const auto translation = glm::mat3{x, y, {node_placement.position.x, node_placement.position.y, 1.F}};

	return translation * rotation_scale * to_pivot;
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

		if(!world.any_of<children>(entity)) {
			continue;
		}

		const auto &parent_world = world.get<transform>(entity).world;
		const auto &parent_placement = world.get<placement>(entity);
		const auto parent_pivot_offset =
			world.all_of<metrics>(entity) ? parent_placement.pivot * world.get<metrics>(entity).size
										  : glm::vec2{0.F, 0.F};

		// The parent's pivot in world space — same calculation render_system uses
		const auto pv = parent_world * glm::vec3{parent_pivot_offset.x, parent_pivot_offset.y, 1.F};
		const auto parent_pos = glm::vec2{pv.x, pv.y};

		for(const auto &kids = world.get<children>(entity).ids; const auto child: kids) {
			const auto &local = world.get<placement>(child);
			const auto child_pivot_offset =
				world.all_of<metrics>(child) ? local.pivot * world.get<metrics>(child).size : glm::vec2{0.F, 0.F};

			// Parent rotation matrix
			const float parent_rad = glm::radians(parent_placement.rotation);
			const float ps = glm::sin(parent_rad);
			const float pc = glm::cos(parent_rad);

			// Child pivot in world space = parent pivot + child local position rotated by parent rotation
			const auto child_pivot_world = parent_pos
				+ glm::vec2{pc * local.position.x + ps * local.position.y,
							-ps * local.position.x + pc * local.position.y};

			// Combined rotation for rendering
			const float combined_rad = glm::radians(parent_placement.rotation + local.rotation);
			const float s = glm::sin(combined_rad);
			const float c = glm::cos(combined_rad);

			// Render pos = child pivot world - RS * child_pivot_offset
			// RS cols are {c,s} and {-s,c}, so RS*p = (c*px - s*py, s*px + c*py)
			const auto render_pos = child_pivot_world
				- glm::vec2{c * child_pivot_offset.x - s * child_pivot_offset.y,
							s * child_pivot_offset.x + c * child_pivot_offset.y};

			const auto child_world_mat = glm::mat3{
				glm::vec3{c * local.scale.x,  s * local.scale.y, 0.F},
				glm::vec3{-s * local.scale.x, c * local.scale.y, 0.F},
				glm::vec3{render_pos, 1.F}
			};

			world.emplace_or_replace<transform>(child, transform{.world = child_world_mat});
			transform_stack_.push_back(child);
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