// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "hello_world.hpp"

#include <lge/app.hpp>
#include <lge/app_config.hpp>
#include <lge/components/label.hpp>
#include <lge/main.hpp>
#include <lge/result.hpp>

#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <spdlog/spdlog.h>

LGE_MAIN(hello_world);

lge::app_config hello_world::configure() {
	return {.design_resolution = {640, 360}, // world expands horizontally keep the vertical size fixed
			.clear_color = {0, 0, 0, 1},	 // black background
			.window_title = "Hello world!"};
}

auto hello_world::init() -> lge::result<> {
	if(const auto err = app::init().unwrap(); err) {
		return lge::error("error init the app", *err);
	}

	auto &world = get_world();
	const auto text = world.create();
	auto &text_label = world.emplace<lge::label>(text, "Hello world!");
	text_label.vertical_align = lge::vertical_alignment::center;
	text_label.horizontal_align = lge::horizontal_alignment::center;
	text_label.color = {1, 1, 0, 1};
	world.emplace<glm::vec2>(text, 0, 0); // center of the world, which is the center of the screen in this case

	SPDLOG_INFO("Hello world");
	return true;
}
