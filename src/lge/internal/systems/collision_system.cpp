// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "collision_system.hpp"

#include <lge/components/collidable.hpp>
#include <lge/core/result.hpp>
#include <lge/events/collision.hpp>
#include <lge/internal/components/bounds.hpp>
#include <lge/internal/components/transform.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <entt/entt.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <vector>

namespace lge {

auto collision_system::update(const float /*dt*/) -> result<> {
	current_collisions_.clear();

	const auto view = ctx.world.view<collidable, bounds, transform>();
	const auto entities = std::vector(view.begin(), view.end());

	for(size_t i = 0; i < entities.size(); ++i) {
		for(size_t j = i + 1; j < entities.size(); ++j) {
			const auto a = entities[i];
			const auto b = entities[j];

			if(overlaps(ctx.world.get<bounds>(a),
						ctx.world.get<transform>(a).world,
						ctx.world.get<bounds>(b),
						ctx.world.get<transform>(b).world)) {
				current_collisions_.push_back({.first = a, .second = b});
			}
		}
	}

	for(const auto &col: current_collisions_) {
		const auto already = std::ranges::any_of(previous_collisions_, [&](const collision &prev) -> bool {
			return (prev.first == col.first && prev.second == col.second)
				   || (prev.first == col.second && prev.second == col.first);
		});
		if(!already) [[likely]] {
			if(const auto err = ctx.events.post(col).unwrap(); err) [[unlikely]] {
				return error("failed to post collision event", *err);
			}
		}
	}

	previous_collisions_ = current_collisions_;
	return true;
}

auto collision_system::to_world(const bounds &b, const glm::mat3 &world) noexcept -> std::array<glm::vec2, 4> {
	const auto transform = [&](const glm::vec2 &p) -> glm::vec2 {
		const glm::vec3 v{p.x, p.y, 1.0F};
		const auto r = world * v;
		return {r.x, r.y};
	};
	return {transform(b.p0), transform(b.p1), transform(b.p2), transform(b.p3)};
}

auto collision_system::point_in_triangle(const glm::vec2 &p,
										 const glm::vec2 &a,
										 const glm::vec2 &b,
										 const glm::vec2 &c) noexcept -> bool {
	const auto d1 = ((p.x - b.x) * (a.y - b.y)) - ((a.x - b.x) * (p.y - b.y));
	const auto d2 = ((p.x - c.x) * (b.y - c.y)) - ((b.x - c.x) * (p.y - c.y));
	const auto d3 = ((p.x - a.x) * (c.y - a.y)) - ((c.x - a.x) * (p.y - a.y));

	const auto has_neg = (d1 < 0.0F) || (d2 < 0.0F) || (d3 < 0.0F);
	const auto has_pos = (d1 > 0.0F) || (d2 > 0.0F) || (d3 > 0.0F);

	return !(has_neg && has_pos);
}

auto collision_system::triangles_intersect(const glm::vec2 &a0,
										   const glm::vec2 &a1,
										   const glm::vec2 &a2,
										   const glm::vec2 &b0,
										   const glm::vec2 &b1,
										   const glm::vec2 &b2) noexcept -> bool {
	return point_in_triangle(b0, a0, a1, a2) || point_in_triangle(b1, a0, a1, a2) || point_in_triangle(b2, a0, a1, a2)
		   || point_in_triangle(a0, b0, b1, b2) || point_in_triangle(a1, b0, b1, b2)
		   || point_in_triangle(a2, b0, b1, b2);
}

auto collision_system::overlaps(const bounds &a,
								const glm::mat3 &a_world,
								const bounds &b,
								const glm::mat3 &b_world) noexcept -> bool {
	const auto wa = to_world(a, a_world);
	const auto wb = to_world(b, b_world);

	// each quad splits into two triangles: (p0,p1,p3) and (p1,p2,p3)
	return triangles_intersect(wa[0], wa[1], wa[3], wb[0], wb[1], wb[3])
		   || triangles_intersect(wa[0], wa[1], wa[3], wb[1], wb[2], wb[3])
		   || triangles_intersect(wa[1], wa[2], wa[3], wb[0], wb[1], wb[3])
		   || triangles_intersect(wa[1], wa[2], wa[3], wb[1], wb[2], wb[3]);
}

} // namespace lge