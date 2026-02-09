// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/label.hpp>
#include <lge/components/metrics.hpp>
#include <lge/components/transform.hpp>
#include <lge/result.hpp>
#include <lge/systems/render_system.hpp>

#include <entt/entt.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>

namespace lge {

auto render_system::update(const float /*dt*/) -> result<> {
	for(const auto entity: world.view<transform, metrics>()) {
		const auto &tf = world.get<transform>(entity);
		const auto &mt = world.get<metrics>(entity);

		// Compute local top-left (relative to pivot)
		const auto top_left_local = -mt.pivot_to_top_left;

		// Transform to world space
		const auto top_left_world = transform_point(tf.world, top_left_local);

		// Draw label if present
		if(world.all_of<label>(entity)) {
			const auto &lbl = world.get<label>(entity);
			renderer_.render_label(lbl.text, static_cast<int>(lbl.size), lbl.color, top_left_world);
		}

		// Debug rectangle will use bounds system later
	}

	return true;
}

auto render_system::transform_point(const glm::mat3 &m, const glm::vec2 &p) -> glm::vec2 {
	const glm::vec3 v{p.x, p.y, 1.f};
	const auto r = m * v;
	return {r.x, r.y};
}

} // namespace lge