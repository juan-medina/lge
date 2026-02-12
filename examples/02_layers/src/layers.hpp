// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/app.hpp>
#include <lge/app_config.hpp>
#include <lge/result.hpp>

#include <entity/entity.hpp>
#include <entity/fwd.hpp>

class layers: public lge::app {
public:
	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto configure() -> lge::app_config override;
	[[nodiscard]] auto update(float dt) -> lge::result<> override;

private:
	static constexpr auto game_res = glm::vec2{640, 360}; // world expands horizontally keep the vertical size fixed
	static constexpr auto clear_color = glm::vec4{0, 0, 0, 1}; // black background
	static constexpr auto game_title = "Layers!";
	static constexpr auto kb_message = "press 1: blue on top, 2: red on top, 3: green on top, "
									   "F11: toggle fullscreen, Esc: exit";
	static constexpr auto controller_message = "controller A: blue on top, X: red on top, Y: green on top, "
											   "SELECT: toggle fullscreen, B: exit";

	static constexpr auto blue_action = "blue";
	static constexpr auto red_action = "red";
	static constexpr auto green_action = "green";
	static constexpr auto fullscreen_action = "fullscreen";
	static constexpr auto exit_action = "exit";

	// rectangle parameters
	static constexpr auto rect_min_width = 30.0F;
	static constexpr auto rect_max_width = 100.0F;
	static constexpr auto rect_min_height = 40.0F;
	static constexpr auto rect_max_height = 60.0F;
	static constexpr auto rect_min_border_padding = 1.0F;
	static constexpr auto rect_max_border_padding = 10.0F;
	static constexpr auto rect_count_per_color = 40;
	static constexpr auto rect_border_thickness = 2.0F;
	static constexpr auto rect_border_color = glm::vec4{0.7F, 0.7F, 0.7F, 1.0F};
	static constexpr auto rect_red_color = glm::vec4{1.0F, 0.0F, 0.0F, 1.0F};
	static constexpr auto rect_green_color = glm::vec4{0.0F, 1.0F, 0.0F, 1.0F};
	static constexpr auto rect_blue_color = glm::vec4{0.0F, 0.0F, 1.0F, 1.0F};

	bool was_in_controller_mode_ = false;
	entt::entity message_ent_ = entt::null;

	auto create_random_rectangles() -> void;
};
