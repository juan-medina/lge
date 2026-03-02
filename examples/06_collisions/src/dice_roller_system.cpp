// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "dice_roller_system.hpp"

#include <lge/components/collidable.hpp>
#include <lge/components/placement.hpp>
#include <lge/components/sprite.hpp>
#include <lge/core/result.hpp>
#include <lge/events/collision.hpp>

#include "dice.hpp"
#include "events.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <entt/entity/fwd.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <numeric>
#include <random>
#include <vector>

namespace examples {

auto dice_roller_system::init() -> lge::result<> {
	if(const auto err = ctx.resources.load_sprite_sheet(dices_path).unwrap(sheet_); err) [[unlikely]] {
		return lge::error("failed to load dice sprite sheet", *err);
	}

	dice_size_ = ctx.render.get_sprite_frame_size(sheet_, faces[0]);

	if(const auto err = ctx.resources.load_sound(dice_throw_sound_path).unwrap(dice_throw_sound_); err) [[unlikely]] {
		return lge::error("failed to load dice throw sound", *err);
	}

	for(size_t i = 0; i < hit_sound_paths.size(); ++i) {
		if(const auto err = ctx.resources.load_sound(hit_sound_paths[i]).unwrap(hit_sounds_[i]); err) [[unlikely]] {
			return lge::error("failed to load hit sound", *err);
		}
	}

	throw_sub_ =
		ctx.events.subscribe<dice_throw>([this](const dice_throw &evt) -> lge::result<> { return on_dice_throw(evt); });

	collision_sub_ = ctx.events.subscribe<lge::collision>(
		[this](const lge::collision &col) -> lge::result<> { return on_collision(col); });

	return true;
}

auto dice_roller_system::end() -> lge::result<> {
	if(const auto err = ctx.events.unsubscribe(collision_sub_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unsubscribe from collision events", *err);
	}
	if(const auto err = ctx.events.unsubscribe(throw_sub_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unsubscribe from dice throw events", *err);
	}

	if(const auto err = ctx.resources.unload_sound(dice_throw_sound_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unload dice throw sound", *err);
	}

	for(const auto handle: hit_sounds_) {
		if(const auto err = ctx.resources.unload_sound(handle).unwrap(); err) [[unlikely]] {
			return lge::error("failed to unload hit sound", *err);
		}
	}

	if(const auto err = ctx.resources.unload_sprite_sheet(sheet_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unload dice sprite sheet", *err);
	}

	return true;
}

auto dice_roller_system::update(const float dt) -> lge::result<> {
	if(hit_cooldown_remaining_ > 0.0F) {
		hit_cooldown_remaining_ -= dt;
	}

	const auto res = ctx.render.get_drawing_resolution();
	const auto half_width = res.x * 0.5F;
	const auto half_height = res.y * 0.5F;

	for(const auto view = ctx.world.view<lge::placement, dice>(); const auto entity: view) {
		auto &d = view.get<dice>(entity);

		if(d.delay > 0.0F) {
			d.delay -= dt;
			continue;
		}

		if(d.settled) {
			continue;
		}

		auto &plc = view.get<lge::placement>(entity);
		plc.position += d.velocity * dt;
		plc.rotation += d.rotation_speed * dt;

		if(d.half_size.x > 0.0F) {
			if(const auto err = bounce_wall(entity, plc, d, half_width, half_height).unwrap(); err) [[unlikely]] {
				return lge::error("failed to bounce wall", *err);
			}
		}

		apply_friction(d, dt);

		if(glm::length(d.velocity) < stop_threshold) {
			if(const auto err = snap_to_rest(plc, d).unwrap(); err) [[unlikely]] {
				return lge::error("failed to snap to rest", *err);
			}
		}
	}

	return true;
}

auto dice_roller_system::on_dice_throw(const dice_throw &evt) -> lge::result<> {
	if(const auto err = ctx.audio.play_sound(dice_throw_sound_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to play dice throw sound", *err);
	}

	dice_in_flight_ = evt.count;
	settled_values_.clear();
	settled_values_.reserve(static_cast<size_t>(evt.count));

	const auto view = ctx.world.view<dice>();
	for(const auto to_destroy = std::vector(view.begin(), view.end()); const auto entity: to_destroy) {
		ctx.world.destroy(entity);
	}

	hit_cooldown_remaining_ = throw_cooldown;

	const auto res = ctx.render.get_drawing_resolution();
	const auto half_width = res.x * 0.5F;
	const auto half_height = res.y * 0.5F;

	const glm::vec2 spawn{half_width + dice_spawn_offset, 0.0F};
	const glm::vec2 target_min{-half_width * board_area, -half_height * board_area};
	const glm::vec2 target_max{half_width * board_area, half_height * board_area};

	for(int i = 0; i < evt.count; ++i) {
		const float jy = std::uniform_real_distribution{-dice_spawn_jitter, dice_spawn_jitter}(rng_);
		const glm::vec2 pos{spawn.x + (dice_size_.x * 2.0F), spawn.y + jy};
		const float delay = static_cast<float>(i) * dice_spawn_delay;
		spawn_die(pos, target_min, target_max, delay);
	}

	return true;
}

auto dice_roller_system::spawn_die(const glm::vec2 pos,
								   const glm::vec2 target_min,
								   const glm::vec2 target_max,
								   const float delay) -> void {
	const float tx = std::uniform_real_distribution{target_min.x, target_max.x}(rng_);
	const float ty = std::uniform_real_distribution{target_min.y, target_max.y}(rng_);
	const auto dir = glm::normalize(glm::vec2{tx, ty} - pos);
	const float speed = std::uniform_real_distribution{min_throw_speed, max_throw_speed}(rng_);

	const auto entity = ctx.world.create();

	const int initial_value = std::uniform_int_distribution{1, 6}(rng_);
	ctx.world.emplace<lge::sprite>(entity, sheet_, faces[static_cast<size_t>(initial_value - 1)]);

	const float rot_speed = std::uniform_real_distribution{min_rotation_speed, max_rotation_speed}(rng_)
							* (std::uniform_int_distribution{0, 1}(rng_) == 0 ? 1.0F : -1.0F);

	ctx.world.emplace<dice>(entity,
							dice{
								.velocity = dir * speed,
								.rotation_speed = rot_speed,
								.delay = delay,
								.half_size = dice_size_ * 0.5F,
								.value = initial_value,
								.settled = false,
							});

	auto &plc = ctx.world.emplace<lge::placement>(entity, pos.x, pos.y);
	plc.rotation = std::uniform_real_distribution{0.0F, 360.0F}(rng_);

	ctx.world.emplace<lge::collidable>(entity);
}

auto dice_roller_system::on_collision(const lge::collision &col) -> lge::result<> {
	const auto res = ctx.render.get_drawing_resolution();
	const auto half_width = res.x * 0.5F;
	const auto half_height = res.y * 0.5F;
	const auto half_dice_w = dice_size_.x * 0.5F;
	const auto half_dice_h = dice_size_.y * 0.5F;

	for(const auto entity: {col.first, col.second}) {
		if(!ctx.world.all_of<dice, lge::placement>(entity)) {
			return true;
		}
		const auto &pos = ctx.world.get<lge::placement>(entity).position;
		const auto off_screen = pos.x - half_dice_w < -half_width || pos.x + half_dice_w > half_width
								|| pos.y - half_dice_h < -half_height || pos.y + half_dice_h > half_height;
		if(off_screen) {
			return true;
		}
	}

	const auto pos_a = ctx.world.get<lge::placement>(col.first).position;
	const auto pos_b = ctx.world.get<lge::placement>(col.second).position;

	if(const auto err = apply_bounce(col.first, glm::normalize(pos_a - pos_b)).unwrap(); err) [[unlikely]] {
		return lge::error("failed to apply bounce to first entity", *err);
	}
	return apply_bounce(col.second, glm::normalize(pos_b - pos_a));
}

auto dice_roller_system::apply_bounce(const entt::entity entity, const glm::vec2 normal) -> lge::result<> {
	auto &d = ctx.world.get<dice>(entity);

	const bool was_at_rest = d.settled;

	d.velocity = (d.velocity - 2.0F * glm::dot(d.velocity, normal) * normal) * damping * bounce_boost;
	d.rotation_speed = -d.rotation_speed * damping * bounce_boost;

	randomise_face(entity, d);

	if(was_at_rest) {
		d.settled = false;
		++dice_in_flight_;
		if(const auto it = std::ranges::find(settled_values_, d.value); it != settled_values_.end()) {
			settled_values_.erase(it);
		}
	}

	if(hit_cooldown_remaining_ > 0.0F) {
		return true;
	}
	if(const auto err = ctx.audio.play_sound(hit_sounds_[next_hit_sound_]).unwrap(); err) [[unlikely]] {
		return lge::error("failed to play hit sound", *err);
	}
	next_hit_sound_ = (next_hit_sound_ + 1) % hit_sounds_.size();
	hit_cooldown_remaining_ = hit_cooldown_time;
	return true;
}

auto dice_roller_system::bounce_wall(const entt::entity entity,
									 lge::placement &plc,
									 const dice &d,
									 const float half_w,
									 const float half_h) -> lge::result<> {
	if(plc.position.x - d.half_size.x < -half_w) {
		plc.position.x = -half_w + d.half_size.x;
		if(const auto err = apply_bounce(entity, wall_normal_left).unwrap(); err) [[unlikely]] {
			return lge::error("failed to apply wall bounce", *err);
		}
	} else if(plc.position.x + d.half_size.x > half_w) {
		plc.position.x = half_w - d.half_size.x;
		if(const auto err = apply_bounce(entity, wall_normal_right).unwrap(); err) [[unlikely]] {
			return lge::error("failed to apply wall bounce", *err);
		}
	}

	if(plc.position.y - d.half_size.y < -half_h) {
		plc.position.y = -half_h + d.half_size.y;
		if(const auto err = apply_bounce(entity, wall_normal_top).unwrap(); err) [[unlikely]] {
			return lge::error("failed to apply wall bounce", *err);
		}
	} else if(plc.position.y + d.half_size.y > half_h) {
		plc.position.y = half_h - d.half_size.y;
		if(const auto err = apply_bounce(entity, wall_normal_bottom).unwrap(); err) [[unlikely]] {
			return lge::error("failed to apply wall bounce", *err);
		}
	}

	return true;
}

auto dice_roller_system::apply_friction(dice &d, const float dt) -> void {
	d.velocity *= (1.0F - (friction * dt));
	d.rotation_speed *= (1.0F - (friction * dt));
}

auto dice_roller_system::snap_to_rest(lge::placement &plc, dice &d) -> lge::result<> {
	d.velocity = {0.0F, 0.0F};
	d.settled = true;

	plc.rotation = d.rotation_speed >= 0.0F ? std::ceil(plc.rotation / snap_step) * snap_step
											: std::floor(plc.rotation / snap_step) * snap_step;

	d.rotation_speed = 0.0F;

	settled_values_.push_back(d.value);
	--dice_in_flight_;

	return check_roll_complete();
}

auto dice_roller_system::check_roll_complete() -> lge::result<> {
	if(dice_in_flight_ > 0 || settled_values_.empty()) {
		return true;
	}

	const auto total = std::accumulate(settled_values_.begin(), settled_values_.end(), 0);

	if(const auto err = ctx.events.post(dice_roll_result{.values = settled_values_, .total = total}).unwrap(); err)
		[[unlikely]] {
		return lge::error("failed to post dice roll result", *err);
	}

	settled_values_.clear();
	return true;
}

auto dice_roller_system::randomise_face(const entt::entity entity, dice &d) -> void {
	const auto prev = d.value;
	while(d.value == prev) {
		d.value = std::uniform_int_distribution{1, 6}(rng_);
	}
	ctx.world.get<lge::sprite>(entity).frame = faces[static_cast<size_t>(d.value - 1)];
}

} // namespace examples