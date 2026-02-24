// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT
#include "menu_scene.hpp"

#include <lge/components/label.hpp>
#include <lge/components/placement.hpp>
#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>
#include <lge/scene/scene.hpp>

auto menu_scene::init() -> lge::result<> {
	ctx.actions.bind(go_to_game_action,
					 {
						 .keys = {lge::input::key::one},
						 .buttons = {lge::input::button::right_face_down},
					 });

	return true;
}

auto menu_scene::on_enter() -> lge::result<> {
	const auto text_label = ctx.world.create();
	ctx.world.emplace<lge::label>(text_label, "Menu Scene", lge::colors::cyan);
	ctx.world.emplace<lge::placement>(text_label);

	return true;
}

auto menu_scene::update(const float dt) -> lge::result<> {
	if(ctx.actions.get(go_to_game_action).pressed) {
		if(const auto err = ctx.events.post(go_to_game{}).unwrap(); err) [[unlikely]] {
			return lge::error("failed to post go_to_game event", *err);
		}
	}

	return scene::update(dt);
}
