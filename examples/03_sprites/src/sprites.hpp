// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include "../../src/example.hpp"

#include <array>
#include <cstddef>
#include <entt/core/fwd.hpp>
#include <entt/core/hashed_string.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <glm/ext/vector_float2.hpp>

namespace examples {

using entt::literals::operator""_hs;

class sprites: public example {
public:
	explicit sprites(): example(example_title, kb_message, controller_message, custom_clear_color) {}
	[[nodiscard]] auto update(float dt) -> lge::result<> override;

protected:
	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto end() -> lge::result<> override;

private:
	static constexpr auto custom_clear_color = lge::color::from_hex(0x1E303CFF);
	static constexpr auto bands_color = lge::color::from_hex(0xDA5E53FF);

	static constexpr auto example_title = "Sprites!";
	static constexpr auto kb_message = "Cursor left/right to move, F5 debug draw,\n"
									   "F11: toggle fullscreen, Esc: exit";
	static constexpr auto controller_message = "Controller left/right to move,\n"
											   "START: debug draw, SELECT: toggle fullscreen, B: exit";

	static constexpr auto anim_path = "resources/game/sprites/hiker_anim.json";

	static constexpr auto idle_anim = "idle"_hs;
	static constexpr auto run_anim = "run"_hs;

	static constexpr size_t left_action = total_base_actions + 0;
	static constexpr size_t right_action = total_base_actions + 1;

	lge::animation_library_handle animation_library_{lge::invalid_animation_library};

	entt::entity sprite_{entt::null};

	static constexpr auto bg_path = "resources/game/sprites/background.json";
	lge::sprite_sheet_handle bg_sheet_{lge::invalid_sprite_sheet};

	static constexpr std::array<entt::hashed_string, 3> bg{
		"parallax-demon-woods-far-trees.png"_hs,
		"parallax-demon-woods-mid-trees.png"_hs,
		"parallax-demon-woods-close-trees.png"_hs,
	};

	static constexpr std::array<float, 3> factors{0.5F, 1.5F, 2.0F};

	glm::vec2 bg_size_{0.0F, 0.0F};
	static constexpr auto run_speed = 45.0F;
	static constexpr auto character_y = 75.0F;

	struct parallax {
		float factor;
	};
};

} // namespace examples