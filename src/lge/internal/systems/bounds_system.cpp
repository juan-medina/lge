// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "bounds_system.hpp"

#include <lge/components/placement.hpp>
#include <lge/core/result.hpp>
#include <lge/internal/components/bounds.hpp>
#include <lge/internal/components/metrics.hpp>

#include <entt/entt.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float2.hpp>

namespace lge {

auto bounds_system::update(const float /*dt*/) -> result<> {
	for(const auto entity: world.view<metrics, placement>()) {
		const auto &m = world.get<metrics>(entity);
		const auto &plc = world.get<placement>(entity);

		const auto pivot_to_top_left = -plc.pivot * m.size;
		const auto top_left = pivot_to_top_left;

		world.emplace_or_replace<bounds>(entity,
										 bounds{
											 .p0 = top_left,
											 .p1 = top_left + glm::vec2{m.size.x, 0.0F},
											 .p2 = top_left + m.size,
											 .p3 = top_left + glm::vec2{0.0F, m.size.y},
										 });
	}
	return true;
}

} // namespace lge