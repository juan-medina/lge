// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "example.hpp"

#include <lge/app/app.hpp>
#include <lge/app/app_config.hpp>
#include <lge/components/label.hpp>
#include <lge/components/order.hpp>
#include <lge/components/placement.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/renderer.hpp>

namespace examples {

auto example::update(const float dt) -> lge::result<> {
	handle_common_input();
	update_controller_mode_message();

	return app::update(dt);
}

auto example::configure() -> lge::app_config {
	return {.design_resolution = game_res, .clear_color = clear_color_, .window_title = game_title_};
}

auto example::init() -> lge::result<> {
	if(const auto err = app::init().unwrap(); err) [[unlikely]] {
		return lge::error("error init the app", *err);
	}

	auto &world = get_world();

	message_ent_ = world.create();
	world.emplace<lge::label>(message_ent_, kb_message_);
	world.emplace<lge::placement>(
		message_ent_, 0.0F, game_res.y / 2, 0.0F, glm::vec2{1.F, 1.F}, lge::pivot::bottom_center);

	world.emplace<lge::order>(message_ent_, 999); // layer 999: render on top of most things
	bind_common_actions();

	return true;
}

auto example::bind_common_actions() -> void {
	auto &input = get_input();
	input.bind(fullscreen_action,
			   {
				   .keys = {lge::input::key::f11},
				   .buttons = {lge::input::button::middle_left},
			   });

	input.bind(exit_action,
			   {
				   .keys = {lge::input::key::escape},
				   .buttons = {lge::input::button::right_face_right},
			   });
	input.bind(debug_action,
			   {
				   .keys = {lge::input::key::f5},
				   .buttons = {lge::input::button::middle_right},
			   });
}

auto example::handle_common_input() -> void {
	const auto &input = get_input();

	if(input.get(fullscreen_action).pressed) {
		get_renderer().toggle_fullscreen();
	}

	if(input.get(exit_action).pressed) {
		exit();
	}

	if(input.get(debug_action).pressed) {
		get_renderer().toggle_debug_draw();
	}
}

auto example::update_controller_mode_message() -> void {
	if(const auto in_controller_mode = get_input().is_controller_available();
	   in_controller_mode != was_in_controller_mode_) {
		auto &world = get_world();
		auto &message_label = world.get<lge::label>(message_ent_);
		if(in_controller_mode) {
			message_label.text = controller_message_;
		} else {
			message_label.text = kb_message_;
		}
		was_in_controller_mode_ = in_controller_mode;
	}
}

} // namespace examples