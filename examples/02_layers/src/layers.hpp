// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/result.hpp>

#include "../../src/example.hpp"

#include <array>
#include <cstddef>
#include <glm/ext/vector_float2.hpp>
#include <random>

class layers: public example {
public:
	explicit layers(): example(example_title, kb_message, controller_message) {}

	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto update(float dt) -> lge::result<> override;

private:
	static constexpr auto example_title = "Layers!";
	static constexpr auto kb_message = "press 1: blue on top, 2: red on top, 3: green on top, "
									   "F11: toggle fullscreen, Esc: exit";
	static constexpr auto controller_message = "controller A: blue on top, X: red on top, Y: green on top, "
											   "SELECT: toggle fullscreen, B: exit";

	static constexpr auto blue_action = "blue";
	static constexpr auto red_action = "red";
	static constexpr auto green_action = "green";

	// colors for the shapes, with alpha 1.0 for full opacity
	static constexpr auto red_color = glm::vec4{1.0F, 0.0F, 0.0F, 1.0F};
	static constexpr auto green_color = glm::vec4{0.0F, 1.0F, 0.0F, 1.0F};
	static constexpr auto blue_color = glm::vec4{0.0F, 0.0F, 1.0F, 1.0F};
	static constexpr std::array colors = {red_color, green_color, blue_color};

	// randomly interleave colors until we have exactly rect_count_per_color of each
	std::array<size_t, colors.size()> color_counts_ = {0, 0, 0};
	static constexpr auto shapes_count_per_color = 40;
	static constexpr auto shapes_border_thickness = 1.0F;
	static constexpr auto shapes_border_color = glm::vec4{0.7F, 0.7F, 0.7F, 1.0F};

	// shapes
	static constexpr auto shape_rect = 0;
	static constexpr auto shape_circle = 1;

	// rectangle parameters
	static constexpr auto rect_min_width = 30.0F;
	static constexpr auto rect_max_width = 100.0F;
	static constexpr auto rect_min_height = 40.0F;
	static constexpr auto rect_max_height = 60.0F;
	static constexpr auto rect_min_border_padding = 1.0F;
	static constexpr auto rect_max_border_padding = 10.0F;

	// circle parameters
	static constexpr auto circle_min_radius = 15.0F;
	static constexpr auto circle_max_radius = 50.0F;

	auto create_random_shapes() -> void;
	auto create_rectangle(float pos_x, float pos_y, const glm::vec4 &color, std::mt19937 &rng) -> void;
	auto create_circle(float pos_x, float pos_y, const glm::vec4 &color, std::mt19937 &rng) -> void;
};
