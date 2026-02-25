// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "collisions.hpp"

#include <lge/app/main.hpp>
#include <lge/components/placement.hpp>
#include <lge/components/sprite.hpp>
#include <lge/core/result.hpp>
#include <lge/systems/system.hpp>

#include "../../src/example.hpp"
#include "movement.hpp"
#include "movement_system.hpp"

#include <cassert>
#include <cstddef>
#include <entt/entt.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <random>

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

	register_system<movement_system>(lge::phase::game_update);

	return true;
}

auto collisions::update(const float dt) -> lge::result<> {
	if(ctx.actions.get(throw_action).pressed) {
		throw_dices();
	}

	return example::update(dt);
}

auto collisions::end() -> lge::result<> {
	if(const auto err = ctx.resources.unload_sprite_sheet(dices_sheet_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unload dices sprite sheet", *err);
	}

	return example::end();
}

// throw_dices — all from same corner, staggered by time
auto collisions::throw_dices() -> void {
	const auto res = ctx.render.get_drawing_resolution();
	static std::random_device rd;
	static std::mt19937 rng{rd()};

	const auto num_dices = std::uniform_int_distribution{3, 6}(rng);
	const auto half_width  = res.x * 0.5F;
	const auto half_height = res.y * 0.5F;

	// spawn from right edge, vertically centered with small jitter
	const glm::vec2 corner{half_width + dice_spawn_offset, 0.0F};

	const glm::vec2 target_min{-half_width * board_area, -half_height * board_area};
	const glm::vec2 target_max{ half_width * board_area,  half_height * board_area};

	for(int i = 0; i < num_dices; ++i) {
		const float jx = std::uniform_real_distribution{-dice_spawn_jitter, dice_spawn_jitter}(rng);
		const float jy = std::uniform_real_distribution{-dice_spawn_jitter, dice_spawn_jitter}(rng);
		const glm::vec2 pos{corner.x + jx, corner.y + jy};

		const float delay = static_cast<float>(i) * dice_spawn_delay;
		spawn_die(pos, target_min, target_max, delay, rng);
	}
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

auto collisions::throw_dice(const glm::vec2 pos, const glm::vec2 vel,
							 const int value, const float delay, std::mt19937 &rng) -> void {
	const auto dice = ctx.world.create();
	ctx.world.emplace<lge::sprite>(dice, dices_sheet_, dices_face_[static_cast<size_t>(value - 1)]);

	const float rot_speed = std::uniform_real_distribution{min_rotation_speed, max_rotation_speed}(rng)
						  * (std::uniform_int_distribution{0, 1}(rng) == 0 ? 1.0F : -1.0F);

	ctx.world.emplace<movement>(dice, movement{.velocity = vel, .rotation_speed = rot_speed, .delay = delay});

	auto &plc = ctx.world.emplace<lge::placement>(dice, pos.x, pos.y);
	plc.rotation = std::uniform_real_distribution{0.0F, 360.0F}(rng);
}

} // namespace examples