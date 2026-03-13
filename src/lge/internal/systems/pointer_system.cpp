// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "pointer_system.hpp"

#include <lge/components/clickable.hpp>
#include <lge/components/hovered.hpp>
#include <lge/core/result.hpp>
#include <lge/events/click.hpp>
#include <lge/interface/renderer.hpp>
#include <lge/internal/components/bounds.hpp>
#include <lge/internal/components/effective_hidden.hpp>
#include <lge/internal/components/pressed.hpp>

#include <array>
#include <cstddef>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <glm/ext/vector_float2.hpp>

namespace lge {

auto pointer_system::update(const float /*dt*/) -> result<> {
	const auto mouse = ctx.actions.get_mouse_position();
	const auto mouse_down = ctx.actions.is_mouse_button_pressed(0);

	for(const auto entity: ctx.world.view<hovered>()) {
		ctx.world.remove<hovered>(entity);
	}

	const auto view = ctx.world.view<clickable, bounds>(entt::exclude<effective_hidden>);

	auto cursor = cursor_type::arrow;

	for(const auto entity: view) {
		const auto &b = ctx.world.get<bounds>(entity);
		const auto quad = std::array<glm::vec2, 4>{b.p0, b.p1, b.p2, b.p3};
		const auto inside = point_in_quad(mouse, quad);

		if(!inside) [[likely]] {
			ctx.world.remove<pressed>(entity);
			continue;
		}

		ctx.world.emplace_or_replace<hovered>(entity);
		cursor = cursor_type::hand;

		if(mouse_down) [[unlikely]] {
			ctx.world.emplace_or_replace<pressed>(entity);
		} else {
			// mouse button released while hovered — fire click
			if(ctx.world.all_of<pressed>(entity)) [[unlikely]] {
				ctx.world.remove<pressed>(entity);
				if(const auto err = ctx.events.post(click{.entity = entity}).unwrap(); err) [[unlikely]] {
					return error("failed to post click event", *err);
				}
			}
		}
	}

	ctx.render.set_cursor(cursor);
	return true;
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