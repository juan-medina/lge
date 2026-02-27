// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "bounds_system.hpp"

#include <lge/components/placement.hpp>
#include <lge/core/result.hpp>
#include <lge/internal/components/bounds.hpp>
#include <lge/internal/components/metrics.hpp>
#include <lge/internal/components/transform.hpp>

#include <cmath>
#include <entt/entt.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>

namespace lge {

auto bounds_system::update(const float /*dt*/) -> result<> {
	for(const auto entity: ctx.world.view<metrics, placement, transform>()) {
		const auto &m = ctx.world.get<metrics>(entity);
		const auto &plc = ctx.world.get<placement>(entity);
		const auto &tf = ctx.world.get<transform>(entity);

		// Pivot in world space — identical to what render_system does for every shape.
		const auto pivot_world = transform_point(tf.world, plc.pivot * m.size);

		// Scale and rotation extracted from the world matrix.
		const auto world_scale = get_scale(tf.world);
		const auto rotation_rad = get_rotation_rad(tf.world);
		const auto cr = std::cos(rotation_rad);
		const auto sr = std::sin(rotation_rad);

		// Local corners relative to pivot (same as the old bounds_system local coords).
		const auto pivot_to_top_left = -plc.pivot * m.size;
		const auto lp0 = pivot_to_top_left;
		const auto lp1 = pivot_to_top_left + glm::vec2{m.size.x, 0.F};
		const auto lp2 = pivot_to_top_left + m.size;
		const auto lp3 = pivot_to_top_left + glm::vec2{0.F, m.size.y};

		// Scale then rotate each corner around the world-space pivot.
		// Matches render_system::handle_bounds::transform_local exactly.
		const auto to_world = [&](const glm::vec2 &lp) noexcept -> glm::vec2 {
			const auto scaled = lp * world_scale;
			return pivot_world + glm::vec2{(scaled.x * cr) - (scaled.y * sr), (scaled.x * sr) + (scaled.y * cr)};
		};

		ctx.world.emplace_or_replace<bounds>(entity,
											 bounds{
												 .p0 = to_world(lp0),
												 .p1 = to_world(lp1),
												 .p2 = to_world(lp2),
												 .p3 = to_world(lp3),
											 });
	}
	return true;
}

auto bounds_system::transform_point(const glm::mat3 &m, const glm::vec2 &p) noexcept -> glm::vec2 {
	const auto r = m * glm::vec3{p.x, p.y, 1.F};
	return {r.x, r.y};
}

auto bounds_system::get_scale(const glm::mat3 &m) noexcept -> glm::vec2 {
	return {
		glm::length(glm::vec2{m[0][0], m[0][1]}),
		glm::length(glm::vec2{m[1][0], m[1][1]}),
	};
}

auto bounds_system::get_rotation_rad(const glm::mat3 &m) noexcept -> float {
	const auto sx = glm::length(glm::vec2{m[0][0], m[1][0]});
	return std::atan2(m[1][0] / sx, m[0][0] / sx);
}

} // namespace lge