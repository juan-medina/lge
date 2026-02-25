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

auto collision_system::overlaps(const bounds &a,
								const glm::mat3 &a_world,
								const bounds &b,
								const glm::mat3 &b_world) noexcept -> bool {
	const auto wa = to_world(a, a_world);
	const auto wb = to_world(b, b_world);

	const auto a_min_x = std::min({wa[0].x, wa[1].x, wa[2].x, wa[3].x});
	const auto a_max_x = std::max({wa[0].x, wa[1].x, wa[2].x, wa[3].x});
	const auto a_min_y = std::min({wa[0].y, wa[1].y, wa[2].y, wa[3].y});
	const auto a_max_y = std::max({wa[0].y, wa[1].y, wa[2].y, wa[3].y});

	const auto b_min_x = std::min({wb[0].x, wb[1].x, wb[2].x, wb[3].x});
	const auto b_max_x = std::max({wb[0].x, wb[1].x, wb[2].x, wb[3].x});
	const auto b_min_y = std::min({wb[0].y, wb[1].y, wb[2].y, wb[3].y});
	const auto b_max_y = std::max({wb[0].y, wb[1].y, wb[2].y, wb[3].y});

	return a_max_x > b_min_x && a_min_x < b_max_x && a_max_y > b_min_y && a_min_y < b_max_y;
}

} // namespace lge