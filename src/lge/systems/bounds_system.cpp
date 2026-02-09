// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/bounds.hpp>
#include <lge/components/metrics.hpp>
#include <lge/systems/bounds_system.hpp>

#include <entt/entt.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float2.hpp>

namespace lge {

auto bounds_system::update(const float /*dt*/) -> result<> {
	for(const auto view = world.view<metrics>(); const auto entity: view) {
		const auto &[size, pivot_to_top_left] = view.get<metrics>(entity);
		world.emplace_or_replace<bounds>(entity,
										 bounds{
											 .p0 = -pivot_to_top_left,
											 .p1 = -pivot_to_top_left + glm::vec2{size.x, 0.0f},
											 .p2 = -pivot_to_top_left + size,
											 .p3 = -pivot_to_top_left + glm::vec2{0.0f, size.y},
										 });
	}
	return true;
}

} // namespace lge