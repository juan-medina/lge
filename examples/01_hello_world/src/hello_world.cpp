// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "hello_world.hpp"

#include <lge/components/label.hpp>
#include <lge/main.hpp>
#include <lge/result.hpp>

#include <glm/glm.hpp>
#include <iostream>
#include <spdlog/spdlog.h>

LGE_MAIN(hello_world);

auto hello_world::init() -> lge::result<> {
	if(const auto err = app::init().unwrap(); err) {
		return lge::error("error init the app", *err);
	}

	auto &world = get_world();
	const auto text = world.create();
	world.emplace<lge::label>(text, "Hello world!", glm::vec4{1, 1, 1, 1}, 24.0F);
	world.emplace<glm::vec2>(text, 10, 10);

	SPDLOG_INFO("Hello world");
	return true;
}