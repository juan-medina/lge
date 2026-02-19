// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/components/sprite_animation.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include "../../src/example.hpp"

#include <cstdint>
#include <entity/entity.hpp>
#include <entity/fwd.hpp>
#include <vector>

class sprites: public example {
public:
	explicit sprites(): example(example_title, kb_message, controller_message) {}
	[[nodiscard]] auto update(float dt) -> lge::result<> override;

protected:
	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto end() -> lge::result<> override;

private:
	static constexpr auto example_title = "Sprites!";
	static constexpr auto kb_message = "Press space: to change animation, F5 debug draw,\n"
									   "F11: toggle fullscreen, Esc: exit";
	static constexpr auto controller_message = "Press controller A: to change animation,\n"
											   "START: debug draw, SELECT: toggle fullscreen, B: exit";

	static constexpr auto sheet_path = "resources/game/sprites/hiker.json";

	static constexpr auto idle_frame_name = "idle_";
	static constexpr auto idle_frames = 6;
	static constexpr auto idle_fps = 4.F;
	std::vector<std::string> idle_frames_ = lge::make_frames(idle_frame_name, idle_frames);

	static constexpr auto run_frame_name = "run_";
	static constexpr auto run_frames = 6;
	static constexpr auto run_fps = 8.F;
	std::vector<std::string> run_frames_ = lge::make_frames(run_frame_name, run_frames);

	static constexpr auto change_anim_action = "change_anim";

	enum class animation_state : std::uint8_t {
		idle,
		run,
	} anim_state_ = animation_state::idle;

	lge::sprite_sheet_handle sheet_{lge::invalid_sprite_sheet};
	entt::entity sprite_{entt::null};
};