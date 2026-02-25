// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT
#include "game_scene.hpp"

#include "lge/scene/scene.hpp"
#include <lge/components/label.hpp>
#include <lge/components/placement.hpp>
#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>

#include "../actions.hpp"
#include "../events.hpp"

namespace examples {

auto game_scene::init() -> lge::result<> {
	ctx.actions.bind(actions::go_to_menu_action,
					 {
						 .keys = {lge::input::key::backspace},
						 .buttons = {lge::input::button::right_face_up},
					 });

	return true;
}

auto game_scene::on_enter(game_type type) -> lge::result<> {
	menu_message_ent_ = create_entity();
	auto &message = ctx.world.emplace<lge::label>(menu_message_ent_, kb_message, lge::colors::white);
	auto &mp = ctx.world.emplace<lge::placement>(menu_message_ent_, 0.0F, 100.0F);
	mp.pivot = lge::pivot::center;
	message.text = ctx.actions.is_controller_available() ? controller_message : kb_message;

	const auto text_label = create_entity();
	auto &l = ctx.world.emplace<lge::label>(text_label, "Game Scene", lge::colors::magenta, 34.0F);
	ctx.world.emplace<lge::placement>(text_label);

	switch(type) {
	case game_type::red:
		l.text = "Red Game";
		l.text_color = lge::colors::light_red;
		ctx.render.set_clear_color(lge::colors::dark_red);
		break;
	case game_type::blue:
		l.text = "Blue Game";
		l.text_color = lge::colors::light_blue;
		ctx.render.set_clear_color(lge::colors::dark_blue);
		break;
	}

	return true;
}

auto game_scene::update(float dt) -> lge::result<> {
	if(ctx.actions.get(actions::go_to_menu_action).pressed) {
		if(const auto err = ctx.events.post(go_to_menu{}).unwrap(); err) [[unlikely]] {
			return lge::error("failed to post go_to_menu event", *err);
		}
	}

	ctx.world.get<lge::label>(menu_message_ent_).text =
		ctx.actions.is_controller_available() ? controller_message : kb_message;

	return scene::update(dt);
}

} // namespace examples
