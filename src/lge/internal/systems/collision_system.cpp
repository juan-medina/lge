// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "collision_system.hpp"

#include <lge/components/collidable.hpp>
#include <lge/components/placement.hpp>
#include <lge/core/result.hpp>
#include <lge/events/collision.hpp>
#include <lge/internal/components/bounds.hpp>
#include <lge/internal/components/metrics.hpp>
#include <lge/internal/components/overlapping.hpp>
#include <lge/internal/components/transform.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <entt/entt.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <limits>
#include <vector>

namespace lge {

auto collision_system::update(const float /*dt*/) -> result<> {
	current_collisions_.clear();

	const auto view = ctx.world.view<collidable, bounds, transform, placement, metrics>();
	const auto entities = std::vector(view.begin(), view.end());

	for(const auto entity: ctx.world.view<overlapping>()) {
		ctx.world.remove<overlapping>(entity);
	}

	for(size_t i = 0; i < entities.size(); ++i) {
		for(size_t j = i + 1; j < entities.size(); ++j) {
			const auto a = entities[i];
			const auto b = entities[j];

			const auto &plc_a = ctx.world.get<placement>(a);
			const auto &met_a = ctx.world.get<metrics>(a);
			const auto &plc_b = ctx.world.get<placement>(b);
			const auto &met_b = ctx.world.get<metrics>(b);

			if(overlaps(ctx.world.get<bounds>(a),
						ctx.world.get<transform>(a).world,
						plc_a.pivot,
						met_a.size,
						ctx.world.get<bounds>(b),
						ctx.world.get<transform>(b).world,
						plc_b.pivot,
						met_b.size)) {
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

auto collision_system::to_world(const bounds &b,
								const glm::mat3 &world,
								const glm::vec2 &pivot,
								const glm::vec2 &size) noexcept -> std::array<glm::vec2, 4> {
	// matches render_system::handle_bounds exactly:
	// compute pivot in world space, then rotate each local corner around it
	const auto pivot_offset = pivot * size;
	const auto pivot_world = glm::vec2{
		world[2][0] + world[0][0] * pivot_offset.x + world[1][0] * pivot_offset.y,
		world[2][1] + world[0][1] * pivot_offset.x + world[1][1] * pivot_offset.y,
	};

	const auto sx = glm::length(glm::vec2{world[0][0], world[0][1]});
	const auto sy = glm::length(glm::vec2{world[1][0], world[1][1]});
	const auto world_scale = glm::vec2{sx, sy};

	const auto rotation_rad = std::atan2(world[1][0] / sx, world[0][0] / sx);
	const auto cr = std::cos(rotation_rad);
	const auto sr = std::sin(rotation_rad);

	const auto xform = [&](const glm::vec2 &p) -> glm::vec2 {
		const auto scaled = p * world_scale;
		return pivot_world + glm::vec2{(scaled.x * cr) - (scaled.y * sr), (scaled.x * sr) + (scaled.y * cr)};
	};

	return {xform(b.p0), xform(b.p1), xform(b.p2), xform(b.p3)};
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
	const std::array<glm::vec2, 3> ta{a0, a1, a2};
	const std::array<glm::vec2, 3> tb{b0, b1, b2};
	return sat_overlap(ta, tb) && sat_overlap(tb, ta);
}

auto collision_system::overlaps(const bounds &a,
								const glm::mat3 &a_world,
								const glm::vec2 &a_pivot,
								const glm::vec2 &a_size,
								const bounds &b,
								const glm::mat3 &b_world,
								const glm::vec2 &b_pivot,
								const glm::vec2 &b_size) noexcept -> bool {
	const auto wa = to_world(a, a_world, a_pivot, a_size);
	const auto wb = to_world(b, b_world, b_pivot, b_size);

	// each quad splits into two triangles: (p0,p1,p3) and (p1,p2,p3)
	return triangles_intersect(wa[0], wa[1], wa[3], wb[0], wb[1], wb[3])
		   || triangles_intersect(wa[0], wa[1], wa[3], wb[1], wb[2], wb[3])
		   || triangles_intersect(wa[1], wa[2], wa[3], wb[0], wb[1], wb[3])
		   || triangles_intersect(wa[1], wa[2], wa[3], wb[1], wb[2], wb[3]);
}

} // namespace lge