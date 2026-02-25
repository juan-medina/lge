// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT
#include "menu_scene.hpp"

#include <lge/components/label.hpp>
#include <lge/components/placement.hpp>
#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>
#include <lge/scene/scene.hpp>

#include "../actions.hpp"
#include "../events.hpp"

namespace examples {
auto menu_scene::init() -> lge::result<> {
	ctx.actions.bind(actions::left_action,
					 {
						 .keys = {lge::input::key::left, lge::input::key::a, lge::input::key::o},
						 .buttons = {lge::input::button::left_face_left},
					 });

	ctx.actions.bind(actions::right_action,
					 {
						 .keys = {lge::input::key::right, lge::input::key::d, lge::input::key::p},
						 .buttons = {lge::input::button::left_face_right},
					 });

	ctx.actions.bind(actions::go_to_game_action,
					 {
						 .keys = {lge::input::key::space, lge::input::key::enter},
						 .buttons = {lge::input::button::right_face_down},
					 });

	return true;
}

auto menu_scene::on_enter() -> lge::result<> {
	menu_message_ent_ = create_entity();
	auto &message = ctx.world.emplace<lge::label>(menu_message_ent_, kb_message, lge::colors::white);
	auto &mp = ctx.world.emplace<lge::placement>(menu_message_ent_, 0.0F, 100.0F);
	mp.pivot = lge::pivot::center;
	message.text = ctx.actions.is_controller_available() ? controller_message : kb_message;

	const auto red_label = create_entity();
	ctx.world.emplace<lge::label>(red_label, "Red game", lge::colors::light_red);
	auto &rp = ctx.world.emplace<lge::placement>(red_label, -70.0F, 0.0F);
	rp.pivot = lge::pivot::center;

	const auto blue_label = create_entity();
	ctx.world.emplace<lge::label>(blue_label, "Blue game", lge::colors::light_blue);
	auto &bp = ctx.world.emplace<lge::placement>(blue_label, 70.0F, 0.0F);
	bp.pivot = lge::pivot::center;

	change_selection(game_type::red);

	ctx.render.set_clear_color(lge::colors::dark_green);

	return true;
}

auto menu_scene::update(const float dt) -> lge::result<> {
	if(ctx.actions.get(actions::left_action).pressed) {
		if(selected_game_ == game_type::blue) {
			change_selection(game_type::red);
		}
	}

	if(ctx.actions.get(actions::right_action).pressed) {
		if(selected_game_ == game_type::red) {
			change_selection(game_type::blue);
		}
	}

	if(ctx.actions.get(actions::go_to_game_action).pressed) {
		if(const auto err = ctx.events.post(go_to_game{selected_game_}).unwrap(); err) [[unlikely]] {
			return lge::error("failed to post go_to_game event", *err);
		}
	}

	ctx.world.get<lge::label>(menu_message_ent_).text =
		ctx.actions.is_controller_available() ? controller_message : kb_message;

	return scene::update(dt);
}

auto menu_scene::change_selection(game_type type) -> void {
	selected_game_ = type;

	for(const auto entity: ctx.world.view<lge::label>()) {
		auto &label = ctx.world.get<lge::label>(entity);
		if(label.text_color == lge::colors::light_red) {
			label.size = (selected_game_ == game_type::red) ? 34.0F : 17.0F;
		} else if(label.text_color == lge::colors::light_blue) {
			label.size = (selected_game_ == game_type::blue) ? 34.0F : 17.0F;
		}
	}
}

} // namespace examples