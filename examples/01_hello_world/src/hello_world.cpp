// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "hello_world.hpp"

#include <lge/app.hpp>
#include <lge/app_config.hpp>
#include <lge/components/dirty.hpp>
#include <lge/components/hierarchy.hpp>
#include <lge/components/label.hpp>
#include <lge/components/position.hpp>
#include <lge/main.hpp>
#include <lge/renderer.hpp>
#include <lge/result.hpp>
#include <lge/systems/system.hpp>

#include "move_random_system.hpp"

#include <raylib.h>

LGE_MAIN(hello_world);

auto hello_world::configure() -> lge::app_config {
	return {.design_resolution = game_res, .clear_color = clear_color, .window_title = game_title};
}
auto hello_world::init() -> lge::result<> {
	if(const auto err = app::init().unwrap(); err) {
		return lge::error("error init the app", *err);
	}

	auto &world = get_world();
	const auto hello_text = world.create();
	auto &hello_label = world.emplace<lge::label>(hello_text, "Hello");
	hello_label.vertical_align = lge::vertical_alignment::center;
	hello_label.horizontal_align = lge::horizontal_alignment::center;
	hello_label.color = {1, 1, 0, 1};
	world.emplace<lge::local_position>(hello_text, 0, 0); // center of the world, which is the center of the screen
	world.emplace<move_random_system::tag>(hello_text);	  // mark the entity to be moved by the move_random_system
	world.emplace<lge::dirty>(hello_text); // mark the entity as dirty, so is aabb is calculated, and also it child

	const auto world_text = world.create();
	auto &world_label = world.emplace<lge::label>(world_text, "World");
	world_label.vertical_align = lge::vertical_alignment::center;
	world_label.horizontal_align = lge::horizontal_alignment::center;
	world_label.color = {0, 1, 1, 1};
	world.emplace<lge::local_position>(world_text, 0, 20);
	lge::attach(world, hello_text, world_text); // child of hello

	const auto message_text = world.create();
	auto &message_label = world.emplace<lge::label>(message_text, message);
	message_label.size = 12;
	message_label.vertical_align = lge::vertical_alignment::bottom;
	message_label.horizontal_align = lge::horizontal_alignment::center;
	world.emplace<lge::local_position>(message_text, 0.0F, game_res.y / 2); // bottom center of the world

	register_system<move_random_system>(lge::phase::update);
	return true;
}

auto hello_world::update(const float dt) -> lge::result<> {
	if(IsKeyPressed(KEY_F5)) {
		get_renderer().toggle_debug_draw();
	}

	if(IsKeyPressed(KEY_F11)) {
		lge::renderer::toggle_fullscreen();
	}
	return app::update(dt);
}