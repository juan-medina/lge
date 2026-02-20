// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include "../../src/example.hpp"

#include <cstddef>
#include <entt/core/hashed_string.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>

namespace examples {

using entt::literals::operator""_hs;

class sprites: public example {
public:
	explicit sprites(): example(example_title, kb_message, controller_message) {}
	[[nodiscard]] auto update(float dt) -> lge::result<> override;

protected:
	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto end() -> lge::result<> override;

private:
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

	lge::sprite_sheet_handle sheet_{lge::invalid_sprite_sheet};

	lge::animation_library_handle animation_library_{lge::invalid_animation_library};

	entt::entity sprite_{entt::null};
};

} // namespace examples