// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/bounds.hpp>
#include <lge/components/label.hpp>
#include <lge/components/metrics.hpp>
#include <lge/components/transform.hpp>
#include <lge/result.hpp>
#include <lge/systems/render_system.hpp>

#include <cmath>
#include <entt/entt.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

namespace lge {

auto render_system::update(const float /*dt*/) -> result<> {
	for(const auto entity: world.view<transform, metrics>()) {
		const auto &[world_transform] = world.get<transform>(entity);

		// Draw label if present
		if(world.all_of<label>(entity)) {
			// Get the pivot to top left offset from metrics
			const auto &[size, pivot_to_top_left] = world.get<metrics>(entity);

			// Transform to world space, text is always drawn from the top left corner
			const auto top_left_world = transform_point(world_transform, -pivot_to_top_left);

			const auto &lbl = world.get<label>(entity);
			const auto rotation = get_rotation(world_transform);
			const auto world_scale = get_scale(world_transform);
			const auto final_font_size = lbl.size * world_scale.y;
			renderer_.render_label(lbl.text, static_cast<int>(final_font_size), lbl.color, top_left_world, rotation);
		}

		// Debug draw bounds if present
		if(world.all_of<bounds>(entity) && renderer_.is_debug_draw()) {
			const auto &[p0, p1, p2, p3] = world.get<bounds>(entity);
			renderer_.render_quad(transform_point(world_transform, p0),
								  transform_point(world_transform, p1),
								  transform_point(world_transform, p2),
								  transform_point(world_transform, p3),
								  {1, 0, 0, 0.5F});
		}
	}

	return true;
}

auto render_system::transform_point(const glm::mat3 &m, const glm::vec2 &p) -> glm::vec2 {
	const glm::vec3 v{p.x, p.y, 1.F};
	const auto r = m * v;
	return {r.x, r.y};
}

auto render_system::get_rotation(const glm::mat3 &m) -> float {
	// column 0 = rotated + scaled X axis
	const auto sx = glm::length(glm::vec2{m[0][0], m[1][0]});
	const auto angle = std::atan2(m[1][0] / sx, m[0][0] / sx);
	return glm::degrees(angle); // CCW, math standard
}

auto render_system::get_scale(const glm::mat3 &m) -> glm::vec2 {
	const auto sx = glm::length(glm::vec2{m[0][0], m[0][1]});
	const auto sy = glm::length(glm::vec2{m[1][0], m[1][1]});
	return {sx, sy};
}

} // namespace lge