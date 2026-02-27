// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "collision_system.hpp"

#include <lge/components/collidable.hpp>
#include <lge/core/result.hpp>
#include <lge/events/collision.hpp>
#include <lge/internal/components/bounds.hpp>
#include <lge/internal/components/overlapping.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <entt/entt.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <limits>
#include <vector>

namespace lge {

auto collision_system::update(const float /*dt*/) -> result<> {
	current_collisions_.clear();

	const auto view = ctx.world.view<collidable, bounds>();
	const auto entities = std::vector(view.begin(), view.end());

	for(const auto entity: ctx.world.view<overlapping>()) {
		ctx.world.remove<overlapping>(entity);
	}

	for(size_t i = 0; i < entities.size(); ++i) {
		for(size_t j = i + 1; j < entities.size(); ++j) {
			const auto a = entities[i];
			const auto b = entities[j];

			if(overlaps(ctx.world.get<bounds>(a), ctx.world.get<bounds>(b))) {
				ctx.world.emplace_or_replace<overlapping>(a);
				ctx.world.emplace_or_replace<overlapping>(b);
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

auto collision_system::sat_overlap(const std::array<glm::vec2, 3> &tri_a,
								   const std::array<glm::vec2, 3> &tri_b) noexcept -> bool {
	for(size_t i = 0; i < 3; ++i) {
		const auto edge = tri_a[(i + 1) % 3] - tri_a[i];
		const auto axis = glm::vec2{-edge.y, edge.x};

		auto min_a = std::numeric_limits<float>::max();
		auto max_a = std::numeric_limits<float>::lowest();
		for(const auto &v: tri_a) {
			const auto proj = glm::dot(v, axis);
			min_a = std::min(min_a, proj);
			max_a = std::max(max_a, proj);
		}

		auto min_b = std::numeric_limits<float>::max();
		auto max_b = std::numeric_limits<float>::lowest();
		for(const auto &v: tri_b) {
			const auto proj = glm::dot(v, axis);
			min_b = std::min(min_b, proj);
			max_b = std::max(max_b, proj);
		}

		if(max_a < min_b || max_b < min_a) {
			return false;
		}
	}
	return true;
}

auto collision_system::triangles_intersect(const glm::vec2 &a0,
										   const glm::vec2 &a1,
										   const glm::vec2 &a2,
										   const glm::vec2 &b0,
										   const glm::vec2 &b1,
										   const glm::vec2 &b2) noexcept -> bool {
	const std::array ta{a0, a1, a2};
	const std::array tb{b0, b1, b2};
	return sat_overlap(ta, tb) && sat_overlap(tb, ta);
}

auto collision_system::overlaps(const bounds &a, const bounds &b) noexcept -> bool {
	// each world-space quad splits into two triangles: (p0,p1,p3) and (p1,p2,p3)
	return triangles_intersect(a.p0, a.p1, a.p3, b.p0, b.p1, b.p3)
		   || triangles_intersect(a.p0, a.p1, a.p3, b.p1, b.p2, b.p3)
		   || triangles_intersect(a.p1, a.p2, a.p3, b.p0, b.p1, b.p3)
		   || triangles_intersect(a.p1, a.p2, a.p3, b.p1, b.p2, b.p3);
}

} // namespace lge