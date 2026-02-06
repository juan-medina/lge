// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "hello_world.hpp"

#include <lge/app.hpp>
#include <lge/app_config.hpp>
#include <lge/components/hierarchy.hpp>
#include <lge/components/label.hpp>
#include <lge/components/position.hpp>
#include <lge/main.hpp>
#include <lge/result.hpp>

#include "move_random_system.hpp"

LGE_MAIN(hello_world);

auto hello_world::configure() -> lge::app_config {
	return {.design_resolution = {640, 360}, // world expands horizontally keep the vertical size fixed
			.clear_color = {0, 0, 0, 1},	 // black background
			.window_title = "Hello world!"};
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
	world.emplace<move_random_system::tag>(hello_text);

	const auto world_text = world.create();
	auto &world_label = world.emplace<lge::label>(world_text, "World");
	world_label.vertical_align = lge::vertical_alignment::center;
	world_label.horizontal_align = lge::horizontal_alignment::center;
	world_label.color = {0, 1, 1, 1};
	world.emplace<lge::local_position>(world_text, 0, 20);
	lge::attach(world, hello_text, world_text); // world a child of hello
	world.emplace<move_random_system::tag>(world_text);

	register_system<move_random_system>();
	return true;
}
