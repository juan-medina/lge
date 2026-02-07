// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/app.hpp>
#include <lge/app_config.hpp>
#include <lge/result.hpp>

#include <glm/vec2.hpp>

class hello_world: public lge::app {
public:
	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto configure() -> lge::app_config override;
	[[nodiscard]] auto update(float dt) -> lge::result<> override;

private:
	static constexpr auto game_res = glm::vec2{640, 360}; // world expands horizontally keep the vertical size fixed
	static constexpr auto clear_color = glm::vec4{0, 0, 0, 1}; // black background
	static constexpr auto game_title = "Hello world!";
	static constexpr auto message = "press 1 or 2 to hide parent or children, F5 to toggle debug draw, F11 to toggle fullscreen or Esc to exit";
};
