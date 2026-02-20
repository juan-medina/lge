// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>

#include "../../src/example.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <entity/entity.hpp>
#include <entity/fwd.hpp>
#include <random>

namespace examples {

class layers: public example {
public:
	explicit layers(): example(example_title, kb_message, controller_message) {}

	[[nodiscard]] auto update(float dt) -> lge::result<> override;

protected:
	[[nodiscard]] auto init() -> lge::result<> override;

private:
	static constexpr auto example_title = "Layers!";
	static constexpr auto kb_message = "press 1: change color on top, F5 to toggle debug draw,\n"
									   "F11: toggle fullscreen, Esc: exit";
	static constexpr auto controller_message = "controller A: blue on top, START: debug draw,\n"
											   "SELECT: toggle fullscreen, B: exit";

	enum class top_color : std::int8_t {
		none = -1,
		red = 0,
		green = 1,
		blue = 2,
	};

	top_color top_color_ = top_color::none;
	entt::entity top_text_entity_ = entt::null;
	auto change_top_color(top_color new_top_color) -> void;

	static constexpr size_t change_top_action = total_base_actions + 0;

	// root nodes for each color
	entt::entity red_root_ = entt::null;
	entt::entity green_root_ = entt::null;
	entt::entity blue_root_ = entt::null;
	auto create_root() -> entt::entity;

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

	// text area margins
	static constexpr auto text_area_margin = 15.0F;

	auto create_random_shapes() -> void;
	[[nodiscard]] auto create_rectangle(float pos_x, float pos_y, const glm::vec4 &color, std::mt19937 &rng)
		-> entt::entity;
	[[nodiscard]] auto create_circle(float pos_x, float pos_y, const glm::vec4 &color, std::mt19937 &rng)
		-> entt::entity;
};

} // namespace examples