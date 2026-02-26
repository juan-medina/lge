// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>
#include <lge/events/collision.hpp>
#include <lge/interface/resources.hpp>

#include "../../src/actions.hpp"
#include "../../src/example.hpp"
#include "events.hpp"

#include <array>
#include <cstddef>
#include <entt/core/fwd.hpp>
#include <entt/core/hashed_string.hpp>
#include <glm/ext/vector_float2.hpp>
#include <random>
#include <string_view>

namespace examples {

using entt::literals::operator""_hs;

class collisions: public example {
public:
	explicit collisions(): example(example_title, kb_message, controller_message, custom_clear_color) {}
	[[nodiscard]] auto update(float dt) -> lge::result<> override;

protected:
	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto end() -> lge::result<> override;

private:
	static constexpr auto custom_clear_color = lge::color::from_hex(0x1E303CFF);

	static constexpr auto example_title = "Collisions!";
	static constexpr auto kb_message = "Space/Enter: throw dice, \nF5: debug draw, F11: fullscreen, Esc: exit";
	static constexpr auto controller_message = "A: throw dice, \nSTART: debug draw, SELECT: fullscreen, B: exit";

	static constexpr auto dices_path = "resources/game/sprites/dices.json";
	static constexpr auto dice_throw_sound = "resources/game/sounds/dice_throw.wav";
	static constexpr auto dice_hit_sounds = std::array<std::string_view, 2>{
		"resources/game/sounds/dice_hit1.wav",
		"resources/game/sounds/dice_hit2.wav",
	};

	lge::sound_handle dice_throw_sound_{};
	std::array<lge::sound_handle, dice_hit_sounds.size()> dice_hit_sounds_{};
	size_t next_hit_sound_index_ = 0;
	float hit_sound_cooldown_{0.0F};

	static constexpr size_t throw_action = actions::total_base_actions + 0;

	// throw tuning
	static constexpr float min_throw_speed = 650.0F;
	static constexpr float max_throw_speed = 1200.0F;
	static constexpr float min_rotation_speed = 120.0F;
	static constexpr float max_rotation_speed = 400.0F;
	static constexpr float dice_spawn_offset = 40.0F; // how far off-screen the corner spawn is
	static constexpr float dice_spawn_jitter = 5.0F;  // tiny position noise at spawn
	static constexpr float dice_spawn_delay = 0.12F;  // seconds between each die entering
	static constexpr float board_area = 0.6F;		  // fraction of screen that is the "board" target zone
	static constexpr auto min_dices = 10;
	static constexpr auto max_dices = 15;

	lge::sprite_sheet_handle dices_sheet_{};

	[[nodiscard]] auto throw_dices() -> lge::result<>;
	auto throw_dice(glm::vec2 pos, glm::vec2 vel, int value, float delay, std::mt19937 &rng) -> void;
	auto spawn_die(glm::vec2 pos, glm::vec2 target_min, glm::vec2 target_max, float delay, std::mt19937 &rng) -> void;

	std::array<entt::hashed_string, 6> dices_face_{
		"dice_1.png"_hs,
		"dice_2.png"_hs,
		"dice_3.png"_hs,
		"dice_4.png"_hs,
		"dice_5.png"_hs,
		"dice_6.png"_hs,
	};

	auto on_collision(const lge::collision &col) -> lge::result<>;
	auto on_dice_hit(const dice_hit &hit) -> lge::result<>;

	glm::vec2 dice_size_{};
};

} // namespace examples