// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "movement_system.hpp"

#include <lge/components/placement.hpp>
#include <lge/core/result.hpp>

#include "events.hpp"
#include "movement.hpp"

#include <cmath>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>

namespace examples {

auto movement_system::update(const float dt) -> lge::result<> {
	const auto res = ctx.render.get_drawing_resolution();
	const auto half_width = res.x * 0.5F;
	const auto half_height = res.y * 0.5F;

	for(const auto view = ctx.world.view<lge::placement, movement>(); const auto entity: view) {
		auto &mov = view.get<movement>(entity);

		if(mov.delay > 0.0F) {
			mov.delay -= dt;
			continue;
		}
		if(mov.collision_cooldown > 0.0F) {
			mov.collision_cooldown -= dt;
		}

		auto &plc = view.get<lge::placement>(entity);
		plc.position += mov.velocity * dt;
		plc.rotation += mov.rotation_speed * dt;

		if(mov.half_size.x > 0.0F) {
			if(plc.position.x - mov.half_size.x < -half_width) {
				plc.position.x = -half_width + mov.half_size.x;
				if(const auto err = ctx.events.post(dice_hit{.entity = entity, .normal = {1.0F, 0.0F}}).unwrap(); err)
					[[unlikely]] {
					return lge::error("failed to post dice hit event", *err);
				}
			} else if(plc.position.x + mov.half_size.x > half_width) {
				plc.position.x = half_width - mov.half_size.x;
				if(const auto err = ctx.events.post(dice_hit{.entity = entity, .normal = {-1.0F, 0.0F}}).unwrap(); err)
					[[unlikely]] {
					return lge::error("failed to post dice hit event", *err);
				}
			}

			if(plc.position.y - mov.half_size.y < -half_height) {
				plc.position.y = -half_height + mov.half_size.y;
				if(const auto err = ctx.events.post(dice_hit{.entity = entity, .normal = {0.0F, 1.0F}}).unwrap(); err)
					[[unlikely]] {
					return lge::error("failed to post dice hit event", *err);
				}
			} else if(plc.position.y + mov.half_size.y > half_height) {
				plc.position.y = half_height - mov.half_size.y;
				if(const auto err = ctx.events.post(dice_hit{.entity = entity, .normal = {0.0F, -1.0F}}).unwrap(); err)
					[[unlikely]] {
					return lge::error("failed to post dice hit event", *err);
				}
			}
		}

		constexpr auto friction = 0.75F;
		constexpr auto stop_threshold = 5.0F;

		mov.velocity *= (1.0F - (friction * dt));
		mov.rotation_speed *= (1.0F - (friction * dt));

		if(glm::length(mov.velocity) < stop_threshold) {
			mov.velocity = {0.0F, 0.0F};

			constexpr auto step = 22.5F;
			if(mov.rotation_speed >= 0.0F) {
				plc.rotation = std::ceil(plc.rotation / step) * step;
			} else {
				plc.rotation = std::floor(plc.rotation / step) * step;
			}
			mov.rotation_speed = 0.0F;
		}
	}
	return true;
}

} // namespace examples