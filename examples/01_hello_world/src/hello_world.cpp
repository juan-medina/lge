// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "hello_world.hpp"

#include <lge/app.hpp>
#include <lge/app_config.hpp>
#include <lge/components/hierarchy.hpp>
#include <lge/components/label.hpp>
#include <lge/components/placement.hpp>
#include <lge/main.hpp>
#include <lge/renderer.hpp>
#include <lge/result.hpp>
#include <lge/systems/system.hpp>

#include "oscillation_system.hpp"

#include <raylib.h>

LGE_MAIN(hello_world);

auto hello_world::configure() -> lge::app_config {
	return {.design_resolution = game_res, .clear_color = clear_color, .window_title = game_title};
}
auto hello_world::init() -> lge::result<> {
	if(const auto err = app::init().unwrap(); err) {
		return lge::error("error init the app", *err);
	}

	auto &input = get_input();
	input.bind(debug_action,
			   {
				   .keys = {KEY_F5},
				   .buttons = {GAMEPAD_BUTTON_MIDDLE_RIGHT},
			   });
	input.bind(fullscreen_action,
			   {
				   .keys = {KEY_F11},
				   .buttons = {GAMEPAD_BUTTON_MIDDLE_LEFT},
			   });

	input.bind(exit_action,
			   {
				   .keys = {KEY_ESCAPE, KEY_F12},
				   .buttons = {GAMEPAD_BUTTON_RIGHT_FACE_RIGHT},
			   });

	auto &world = get_world();

	const auto center = world.create();
	world.emplace<lge::placement>(center, 0, 0);
	world.emplace<oscillation_system::effect>(center,
											  oscillation_system::effect{
												  .scale = {.min_ = 1.0F, .max = 3.0F, .period = 8.0F},
												  .rotation = {.min_ = 0.0F, .max = 360.0F, .period = 4.0F},
											  });

	static constexpr auto gap_between_labels = 10.0F;

	const auto hello_text = world.create();
	auto &hello_label = world.emplace<lge::label>(hello_text, "Hello");
	hello_label.vertical_align = lge::vertical_alignment::bottom;
	hello_label.horizontal_align = lge::horizontal_alignment::center;
	hello_label.color = {1, 1, 0, 1};
	world.emplace<lge::placement>(hello_text, 0, -gap_between_labels / 2); // above center
	lge::attach(world, center, hello_text); // child of center

	const auto world_text = world.create();
	auto &world_label = world.emplace<lge::label>(world_text, "World");
	world_label.vertical_align = lge::vertical_alignment::top;
	world_label.horizontal_align = lge::horizontal_alignment::center; // bellow center
	world_label.color = {0, 1, 1, 1};
	world.emplace<lge::placement>(world_text, 0, gap_between_labels / 2);
	lge::attach(world, center, world_text); // child of center

	message_ = world.create();
	auto &message_label = world.emplace<lge::label>(message_, kb_message);
	message_label.size = 12;
	message_label.vertical_align = lge::vertical_alignment::bottom;
	message_label.horizontal_align = lge::horizontal_alignment::center;
	world.emplace<lge::placement>(message_, 0.0F, game_res.y / 2); // at the bottom of the screen
	register_system<oscillation_system>(lge::phase::game_update);
	return true;
}

auto hello_world::update(const float dt) -> lge::result<> {
	const auto &input = get_input();

	if(input.get(debug_action).pressed) {
		get_renderer().toggle_debug_draw();
	}

	if(input.get(fullscreen_action).pressed) {
		lge::renderer::toggle_fullscreen();
	}

	if(input.get(exit_action).pressed) {
		exit();
	}

	if(const auto in_controller_mode = input.is_controller_available(); in_controller_mode != was_in_controller_mode_) {
		auto &world = get_world();
		auto &message_label = world.get<lge::label>(message_);
		if(in_controller_mode) {
			message_label.text = controller_message;
		} else {
			message_label.text = kb_message;
		}
		was_in_controller_mode_ = in_controller_mode;
	}

	return app::update(dt);
}