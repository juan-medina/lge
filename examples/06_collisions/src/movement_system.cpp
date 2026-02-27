// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "movement_system.hpp"

#include <lge/components/placement.hpp>
#include <lge/core/result.hpp>

#include "events.hpp"
#include "movement.hpp"

#include <cmath>
#include <entt/entity/fwd.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>

namespace examples {

auto movement_system::bounce_wall(const entt::entity entity,
								  lge::placement &plc,
								  const movement &mov,
								  const float half_width,
								  const float half_height) -> lge::result<> {
	if(plc.position.x - mov.half_size.x < -half_width) {
		plc.position.x = -half_width + mov.half_size.x;
		if(const auto err = ctx.events.post(dice_hit{.entity = entity, .normal = wall_normal_left}).unwrap(); err)
			[[unlikely]] {
			return lge::error("failed to post dice hit event", *err);
		}
	} else if(plc.position.x + mov.half_size.x > half_width) {
		plc.position.x = half_width - mov.half_size.x;
		if(const auto err = ctx.events.post(dice_hit{.entity = entity, .normal = wall_normal_right}).unwrap(); err)
			[[unlikely]] {
			return lge::error("failed to post dice hit event", *err);
		}
	}

	if(plc.position.y - mov.half_size.y < -half_height) {
		plc.position.y = -half_height + mov.half_size.y;
		if(const auto err = ctx.events.post(dice_hit{.entity = entity, .normal = wall_normal_top}).unwrap(); err)
			[[unlikely]] {
			return lge::error("failed to post dice hit event", *err);
		}
	} else if(plc.position.y + mov.half_size.y > half_height) {
		plc.position.y = half_height - mov.half_size.y;
		if(const auto err = ctx.events.post(dice_hit{.entity = entity, .normal = wall_normal_bottom}).unwrap(); err)
			[[unlikely]] {
			return lge::error("failed to post dice hit event", *err);
		}
	}

	return true;
}

auto movement_system::apply_friction(movement &mov, const float dt) -> void {
	mov.velocity *= (1.0F - (friction * dt));
	mov.rotation_speed *= (1.0F - (friction * dt));
}

auto movement_system::snap_to_rest(lge::placement &plc, movement &mov) -> void {
	mov.velocity = {0.0F, 0.0F};

	if(mov.rotation_speed >= 0.0F) {
		plc.rotation = std::ceil(plc.rotation / snap_step) * snap_step;
	} else {
		plc.rotation = std::floor(plc.rotation / snap_step) * snap_step;
	}
	mov.rotation_speed = 0.0F;
}

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

		auto &plc = view.get<lge::placement>(entity);
		plc.position += mov.velocity * dt;
		plc.rotation += mov.rotation_speed * dt;

		if(mov.half_size.x > 0.0F) {
			if(const auto err = bounce_wall(entity, plc, mov, half_width, half_height).unwrap(); err) [[unlikely]] {
				return lge::error("failed to bounce wall", *err);
			}
		}

		apply_friction(mov, dt);

		if(glm::length(mov.velocity) < stop_threshold) {
			snap_to_rest(plc, mov);
		}
	}
	return true;
}

} // namespace examples