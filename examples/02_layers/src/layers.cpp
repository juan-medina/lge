// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "layers.hpp"

#include <lge/app.hpp>
#include <lge/app_config.hpp>
#include <lge/components/label.hpp>
#include <lge/components/placement.hpp>
#include <lge/components/shapes.hpp>
#include <lge/log.hpp>
#include <lge/main.hpp>
#include <lge/renderer.hpp>
#include <lge/result.hpp>

#include <array>
#include <cassert>
#include <cstddef>
#include <random>

LGE_MAIN(layers);

auto layers::configure() -> lge::app_config {
	return {.design_resolution = game_res, .clear_color = clear_color, .window_title = game_title};
}

auto layers::init() -> lge::result<> {
	if(const auto err = app::init().unwrap(); err) {
		return lge::error("error init the app", *err);
	}

	auto &input = get_input();
	input.bind(red_action,
			   {
				   .keys = {lge::input::key::one},
				   .buttons = {lge::input::button::right_face_down},
			   });
	input.bind(green_action,
			   {
				   .keys = {lge::input::key::two},
				   .buttons = {lge::input::button::right_face_left},
			   });
	input.bind(blue_action,
			   {
				   .keys = {lge::input::key::three},
				   .buttons = {lge::input::button::right_face_up},
			   });
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

	auto &world = get_world();

	message_ent_ = world.create();
	auto &message_label = world.emplace<lge::label>(message_ent_, kb_message);
	message_label.size = 12;
	message_label.vertical_align = lge::vertical_alignment::bottom;
	message_label.horizontal_align = lge::horizontal_alignment::center;
	world.emplace<lge::placement>(message_ent_, 0.0F, game_res.y / 2); // at the bottom of the screen

	create_random_rectangles();

	return true;
}

auto layers::update(const float dt) -> lge::result<> {
	auto &world = get_world();
	const auto &input = get_input();

	if(input.get(red_action).pressed) {
		LGE_DEBUG("red action pressed");
	}

	if(input.get(green_action).pressed) {
		LGE_DEBUG("green action pressed");
	}

	if(input.get(blue_action).pressed) {
		LGE_DEBUG("blue action pressed");
	}

	if(input.get(fullscreen_action).pressed) {
		get_renderer().toggle_fullscreen();
	}

	if(input.get(exit_action).pressed) {
		exit();
	}

	// check if we switched between controller and keyboard mode to update the message at the bottom of the screen
	if(const auto in_controller_mode = input.is_controller_available(); in_controller_mode != was_in_controller_mode_) {
		auto &message_label = world.get<lge::label>(message_ent_);
		if(in_controller_mode) {
			message_label.text = controller_message;
		} else {
			message_label.text = kb_message;
		}
		was_in_controller_mode_ = in_controller_mode;
	}

	return app::update(dt);
}

auto layers::create_random_rectangles() -> void {
	auto &world = get_world();
	std::random_device rd;
	std::mt19937 rng{rd()};

	std::uniform_real_distribution size_dist_w(rect_min_width, rect_max_width);
	std::uniform_real_distribution size_dist_h(rect_min_height, rect_max_height);
	std::uniform_real_distribution pos_x_dist(-game_res.x / 2.0F, game_res.x / 2.0F);
	std::uniform_real_distribution pos_y_dist(-game_res.y / 2.0F, game_res.y / 2.0F);
	std::uniform_real_distribution padding_dist(rect_min_border_padding, rect_max_border_padding);

	constexpr std::array colors = {rect_red_color, rect_green_color, rect_blue_color};

	// randomly interleave colors until we have exactly rect_count_per_color of each
	std::array color_counts = {0, 0, 0};
	std::uniform_int_distribution color_dist(0, 2);

	while(color_counts[0] < rect_count_per_color || color_counts[1] < rect_count_per_color
		  || color_counts[2] < rect_count_per_color) {
		// pick a random color that hasn't reached its quota yet
		size_t color_index = color_dist(rng);
		// just to be extra sure, this should never fail, index is between 0 and 2 inclusive, this does not add
		// any penalty on release builds, and it can help catch logic errors during development
		assert(color_index < colors.size());
		while(color_counts[color_index] >= rect_count_per_color) { // NOLINT(*-pro-bounds-constant-array-index)
			color_index = color_dist(rng);
		}

		const auto &color = colors.at(color_index);
		color_counts[color_index]++; // NOLINT(*-pro-bounds-constant-array-index)

		// generate random size and position
		const auto width = size_dist_w(rng);
		const auto height = size_dist_h(rng);
		const auto half_width = width / 2.0F;
		const auto half_height = height / 2.0F;

		auto pos_x = pos_x_dist(rng);
		auto pos_y = pos_y_dist(rng);

		// adjust position if rectangle would go out of bounds, with random padding to avoid edge clustering
		if(pos_x - half_width < -game_res.x / 2.0F) {
			const auto padding = padding_dist(rng);
			pos_x = (-game_res.x / 2.0F) + half_width + padding;
		}
		if(pos_x + half_width > game_res.x / 2.0F) {
			const auto padding = padding_dist(rng);
			pos_x = (game_res.x / 2.0F) - half_width - padding;
		}
		if(pos_y - half_height < -game_res.y / 2.0F) {
			const auto padding = padding_dist(rng);
			pos_y = (-game_res.y / 2.0F) + half_height + padding;
		}
		if(pos_y + half_height > game_res.y / 2.0F) {
			const auto padding = padding_dist(rng);
			pos_y = (game_res.y / 2.0F) - half_height - padding;
		}

		// create rectangle entity with calculated dimensions and position
		const auto rect_ent = world.create();
		auto &rect = world.emplace<lge::rect>(rect_ent);
		rect.from = {-half_width, -half_height};
		rect.to = {half_width, half_height};
		rect.fill_color = color;
		rect.border_color = rect_border_color;
		rect.border_thickness = rect_border_thickness;
		world.emplace<lge::placement>(rect_ent, pos_x, pos_y);
	}
}
