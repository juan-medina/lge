
// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "pointer_system.hpp"

#include <lge/components/clickable.hpp>
#include <lge/components/hovered.hpp>
#include <lge/components/placement.hpp>
#include <lge/core/result.hpp>
#include <lge/events/click.hpp>
#include <lge/interface/renderer.hpp>
#include <lge/internal/components/bounds.hpp>
#include <lge/internal/components/metrics.hpp>
#include <lge/internal/components/transform.hpp>

#include <array>
#include <cmath>
#include <cstddef>
#include <entt/entt.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>

namespace lge {

auto pointer_system::update(const float /*dt*/) -> result<> {
	const auto mouse = ctx.actions.get_mouse_position();
	const auto pressed = ctx.actions.is_mouse_button_pressed(0);

	// remove hovered from all entities that had it last frame
	for(const auto entity: ctx.world.view<hovered>()) {
		ctx.world.remove<hovered>(entity);
	}

	const auto view = ctx.world.view<clickable, bounds, transform, placement, metrics>();

	auto cursor = cursor_type::arrow;

	for(const auto entity: view) {
		const auto &b = ctx.world.get<bounds>(entity);
		const auto &t = ctx.world.get<transform>(entity);
		const auto &plc = ctx.world.get<placement>(entity);
		const auto &met = ctx.world.get<metrics>(entity);

		const auto quad = to_world(b, t.world, plc.pivot, met.size);

		if(!point_in_quad(mouse, quad)) [[likely]] {
			continue;
		}

		ctx.world.emplace<hovered>(entity);
		cursor = cursor_type::hand;

		if(pressed) [[unlikely]] {
			if(const auto err = ctx.events.post(click{.entity = entity}).unwrap(); err) [[unlikely]] {
				return error("failed to post click event", *err);
			}
		}
	}

	ctx.render.set_cursor(cursor);

	return true;
}

auto pointer_system::to_world(const bounds &b,
							  const glm::mat3 &world,
							  const glm::vec2 &pivot,
							  const glm::vec2 &size) noexcept -> std::array<glm::vec2, 4> {
	const auto pivot_offset = pivot * size;
	const auto pivot_world = glm::vec2{
		world[2][0] + (world[0][0] * pivot_offset.x) + (world[1][0] * pivot_offset.y),
		world[2][1] + (world[0][1] * pivot_offset.x) + (world[1][1] * pivot_offset.y),
	};

	const auto sx = glm::length(glm::vec2{world[0][0], world[0][1]});
	const auto sy = glm::length(glm::vec2{world[1][0], world[1][1]});
	const auto world_scale = glm::vec2{sx, sy};

	const auto rotation_rad = std::atan2(world[1][0] / sx, world[0][0] / sx);
	const auto cr = std::cos(rotation_rad);
	const auto sr = std::sin(rotation_rad);

	const auto xform = [&](const glm::vec2 &p) noexcept -> glm::vec2 {
		const auto scaled = p * world_scale;
		return pivot_world + glm::vec2{(scaled.x * cr) - (scaled.y * sr), (scaled.x * sr) + (scaled.y * cr)};
	};

	return {xform(b.p0), xform(b.p1), xform(b.p2), xform(b.p3)};
}

auto pointer_system::point_in_quad(const glm::vec2 &point, const std::array<glm::vec2, 4> &quad) noexcept -> bool {
	// signed area test: point is inside if it's on the same side of all four edges
	// quad is wound consistently (p0 top-left, p1 top-right, p2 bottom-right, p3 bottom-left)
	for(size_t i = 0; i < 4; ++i) {
		const auto &a = quad[i];
		const auto &b = quad[(i + 1) % 4];
		const auto edge = b - a;
		if(const auto to_point = point - a; ((edge.x * to_point.y) - (edge.y * to_point.x)) < 0.0F) {
			return false;
		}
	}
	return true;
}

} // namespace lge