// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT
#include "game_scene.hpp"

#include <lge/components/label.hpp>
#include <lge/components/placement.hpp>
#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>

#include "../actions.hpp"

namespace examples {

auto game_scene::init() -> lge::result<> {
	ctx.actions.bind(actions::go_to_menu_action,
					 {
						 .keys = {lge::input::key::one},
						 .buttons = {lge::input::button::right_face_down},
					 });

	return true;
}

auto game_scene::on_enter() -> lge::result<> {
	const auto text_label = create_entity();
	ctx.world.emplace<lge::label>(text_label, "Game Scene", lge::colors::magenta);
	ctx.world.emplace<lge::placement>(text_label);

	return true;
}

auto game_scene::update(float dt) -> lge::result<> {
	if(ctx.actions.get(actions::go_to_game_action).pressed) {
		if(const auto err = ctx.events.post(go_to_menu{}).unwrap(); err) [[unlikely]] {
			return lge::error("failed to post go_to_menu event", *err);
		}
	}

	return scene::update(dt);
}

} // namespace examples
