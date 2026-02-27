// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "collisions.hpp"

#include <lge/app/main.hpp>
#include <lge/components/collidable.hpp>
#include <lge/components/placement.hpp>
#include <lge/components/sprite.hpp>
#include <lge/core/result.hpp>
#include <lge/events/collision.hpp>
#include <lge/systems/system.hpp>

#include "../../src/example.hpp"
#include "events.hpp"
#include "movement.hpp"
#include "movement_system.hpp"

#include <cstddef>
#include <entt/entt.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <random>
#include <vector>

LGE_MAIN(examples::collisions);

namespace examples {

auto collisions::init() -> lge::result<> {
	if(const auto err = example::init().unwrap(); err) [[unlikely]] {
		return lge::error("error init the app", *err);
	}

	ctx.actions.bind(throw_action,
					 {
						 .keys = {lge::input::key::space, lge::input::key::enter},
						 .buttons = {lge::input::button::left_face_down},
					 });

	if(const auto err = ctx.resources.load_sprite_sheet(dices_path).unwrap(dices_sheet_); err) [[unlikely]] {
		return lge::error("failed to load dices sprite sheet", *err);
	}

	if(const auto err = ctx.resources.load_sound(dice_throw_sound).unwrap(dice_throw_sound_); err) [[unlikely]] {
		return lge::error("failed to load dice throw sound", *err);
	}

	for(size_t i = 0; i < dice_hit_sounds.size(); ++i) {
		const auto &hit_sound = dice_hit_sounds[i];
		if(const auto err = ctx.resources.load_sound(hit_sound).unwrap(dice_hit_sounds_[i]); err) [[unlikely]] {
			return lge::error("failed to load dice hit sound", *err);
		}
	}

	register_system<movement_system>(lge::phase::game_update);

	ctx.events.on<lge::collision>([this](const lge::collision &col) -> lge::result<> { return on_collision(col); });
	ctx.events.on<dice_hit>([this](const dice_hit &hit) -> lge::result<> { return on_dice_hit(hit); });

	dice_size_ = ctx.render.get_sprite_frame_size(dices_sheet_, dices_face_[0]);

	return true;
}

auto collisions::update(const float dt) -> lge::result<> {
	if(ctx.actions.get(throw_action).pressed) {
		if(const auto err = throw_dices().unwrap(); err) [[unlikely]] {
			return lge::error("failed to throw dices", *err);
		}
	}

	if(hit_sound_cooldown_ > 0.0F) {
		hit_sound_cooldown_ -= dt;
	}

	return example::update(dt);
}

auto collisions::end() -> lge::result<> {
	if(const auto err = ctx.resources.unload_sprite_sheet(dices_sheet_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unload dices sprite sheet", *err);
	}

	if(const auto err = ctx.resources.unload_sound(dice_throw_sound_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unload dice throw sound", *err);
	}

	for(const auto dice_hit_sound: dice_hit_sounds_) {
		if(const auto err = ctx.resources.unload_sound(dice_hit_sound).unwrap(); err) [[unlikely]] {
			return lge::error("failed to unload dice hit sound", *err);
		}
	}

	return example::end();
}

// throw_dices — all from same corner, staggered by time
auto collisions::throw_dices() -> lge::result<> {
	ctx.audio.stop_all();
	if(const auto err = ctx.audio.play_sound(dice_throw_sound_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to play dice throw sound", *err);
	}

	// set cooldown to prevent hit sound from playing immediately after throw
	hit_sound_cooldown_ = 0.5F;

	// remove all existing dice before throwing new ones
	const auto view = ctx.world.view<lge::collidable>();
	for(const auto to_destroy = std::vector(view.begin(), view.end()); const auto entity: to_destroy) {
		ctx.world.destroy(entity);
	}
	const auto res = ctx.render.get_drawing_resolution();
	static std::random_device rd;
	static std::mt19937 rng{rd()};

	const auto num_dices = std::uniform_int_distribution{min_dices, max_dices}(rng);
	const auto half_width = res.x * 0.5F;
	const auto half_height = res.y * 0.5F;

	// spawn from right edge, vertically centered with small jitter
	const glm::vec2 corner{half_width + dice_spawn_offset, 0.0F};

	const glm::vec2 target_min{-half_width * board_area, -half_height * board_area};
	const glm::vec2 target_max{half_width * board_area, half_height * board_area};

	for(int i = 0; i < num_dices; ++i) {
		const float jx = dice_size_.x * 2;
		const float jy = std::uniform_real_distribution{-dice_spawn_jitter, dice_spawn_jitter}(rng);
		const glm::vec2 pos{corner.x + jx, corner.y + jy};

		const float delay = static_cast<float>(i) * dice_spawn_delay;
		spawn_die(pos, target_min, target_max, delay, rng);
	}
	return true;
}

auto collisions::spawn_die(const glm::vec2 pos,
						   const glm::vec2 target_min,
						   const glm::vec2 target_max,
						   const float delay,
						   std::mt19937 &rng) -> void {
	const float tx = std::uniform_real_distribution{target_min.x, target_max.x}(rng);
	const float ty = std::uniform_real_distribution{target_min.y, target_max.y}(rng);
	const auto dir = glm::normalize(glm::vec2{tx, ty} - pos);
	const float speed = std::uniform_real_distribution{min_throw_speed, max_throw_speed}(rng);
	const int value = std::uniform_int_distribution{1, 6}(rng);
	throw_dice(pos, dir * speed, value, delay, rng);
}

auto collisions::on_collision(const lge::collision &col) -> lge::result<> {
	const auto res = ctx.render.get_drawing_resolution();
	const auto half_width = res.x * 0.5F;
	const auto half_height = res.y * 0.5F;
	const auto half_dice_w = dice_size_.x * 0.5F;
	const auto half_dice_h = dice_size_.y * 0.5F;

	for(const auto entity: {col.first, col.second}) {
		if(!ctx.world.all_of<movement, lge::placement>(entity)) {
			return true;
		}

		const auto &pos = ctx.world.get<lge::placement>(entity).position;
		const auto off_screen = pos.x - half_dice_w < -half_width || pos.x + half_dice_w > half_width
								|| pos.y - half_dice_h < -half_height || pos.y + half_dice_h > half_height;
		if(off_screen) {
			return true;
		}
	}

	static std::random_device rd;
	static std::mt19937 rng{rd()};

	const auto pos_a = ctx.world.get<lge::placement>(col.first).position;
	const auto pos_b = ctx.world.get<lge::placement>(col.second).position;
	const auto normal_a = glm::normalize(pos_a - pos_b);
	const auto normal_b = -normal_a;

	for(const auto entity: {col.first, col.second}) {
		const int value = std::uniform_int_distribution{1, 6}(rng);
		ctx.world.get<lge::sprite>(entity).frame = dices_face_[static_cast<size_t>(value - 1)];
	}

	if(const auto err = ctx.events.post(dice_hit{.entity = col.first, .normal = normal_a}).unwrap(); err) [[unlikely]] {
		return lge::error("failed to post dice hit event", *err);
	}
	return ctx.events.post(dice_hit{.entity = col.second, .normal = normal_b});
}

auto collisions::on_dice_hit(const dice_hit &hit) -> lge::result<> {
	constexpr auto damping = 0.75F;
	constexpr auto bounce_boost = 1.05F;

	auto &mov = ctx.world.get<movement>(hit.entity);
	const auto n = hit.normal;
	mov.velocity = (mov.velocity - 2.0F * glm::dot(mov.velocity, n) * n) * damping * bounce_boost;
	mov.rotation_speed = -mov.rotation_speed * damping * bounce_boost;

	if(hit_sound_cooldown_ > 0.0F) {
		return true;
	}
	if(const auto err = ctx.audio.play_sound(dice_hit_sounds_[next_hit_sound_index_]).unwrap(); err) [[unlikely]] {
		return lge::error("failed to play dice hit sound", *err);
	}
	next_hit_sound_index_ = (next_hit_sound_index_ + 1) % dice_hit_sounds_.size();
	hit_sound_cooldown_ = 0.1F;
	return true;
}

auto collisions::throw_dice(
	const glm::vec2 pos, const glm::vec2 vel, const int value, const float delay, std::mt19937 &rng) -> void {
	const auto dice = ctx.world.create();
	ctx.world.emplace<lge::sprite>(dice, dices_sheet_, dices_face_[static_cast<size_t>(value - 1)]);

	const float rot_speed = std::uniform_real_distribution{min_rotation_speed, max_rotation_speed}(rng)
							* (std::uniform_int_distribution{0, 1}(rng) == 0 ? 1.0F : -1.0F);

	ctx.world.emplace<movement>(
		dice, movement{.velocity = vel, .rotation_speed = rot_speed, .delay = delay, .half_size = dice_size_ * 0.5F});

	auto &plc = ctx.world.emplace<lge::placement>(dice, pos.x, pos.y);
	plc.rotation = std::uniform_real_distribution{0.0F, 360.0F}(rng);
	ctx.world.emplace<lge::collidable>(dice);
}

} // namespace examples