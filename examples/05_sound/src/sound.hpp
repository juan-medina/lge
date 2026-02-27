// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/interface/resources.hpp>

#include "../../src/actions.hpp"
#include "../../src/example.hpp"

#include <array>
#include <cstddef>
#include <entt/core/fwd.hpp>
#include <entt/core/hashed_string.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <random>

namespace examples {

using entt::literals::operator""_hs;

class sound: public example {
public:
	explicit sound(): example(example_title, kb_message, controller_message) {}

	[[nodiscard]] auto update(float dt) -> lge::result<> override;

protected:
	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto end() -> lge::result<> override;

private:
	static constexpr auto example_title = "Sound!";
	static constexpr auto kb_message = "Space or click dancer to play/stop music,\n"
									   "F5: debug draw, F11: toggle fullscreen, Esc: exit";
	static constexpr auto controller_message = "Controller A to play/stop music,\n"
											   "START: debug draw, SELECT: toggle fullscreen, B: exit";

	static constexpr auto anim_path = "resources/game/sprites/dancer_anim.json";
	static constexpr auto music_path = "resources/game/music/world_wanderer.ogg";
	static constexpr auto scratch_path = "resources/game/sounds/scratch.wav";

	static constexpr auto idle_anim = "idle"_hs;

	static constexpr std::array<entt::hashed_string, 3> dances{
		"slide"_hs,
		"hips"_hs,
		"snap"_hs,
	};

	static constexpr size_t play_action = actions::total_base_actions + 0;

	lge::animation_library_handle animation_library_{lge::invalid_animation_library};
	lge::music_handle music_{lge::invalid_music};

	entt::entity sprite_{entt::null};

	[[nodiscard]] auto get_dance() -> entt::hashed_string;
	[[nodiscard]] auto toggle_music() -> lge::result<>;

	std::random_device rd_;
	std::mt19937 rng_{rd_()};

	lge::sound_handle scratch_sound_{lge::invalid_sound};
};

} // namespace examples