// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "layers.hpp"

#include <lge/components/placement.hpp>
#include <lge/components/shapes.hpp>
#include <lge/log.hpp>
#include <lge/main.hpp>
#include <lge/result.hpp>

#include "../../src/example.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <random>

LGE_MAIN(layers);

auto layers::init() -> lge::result<> {
	if(const auto err = example::init().unwrap(); err) {
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

	create_random_shapes();

	return true;
}

auto layers::update(const float dt) -> lge::result<> {
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

	return example::update(dt);
}

auto layers::create_random_shapes() -> void {
	std::random_device rd;
	std::mt19937 rng{rd()};
	static constexpr auto game_res = get_game_res();

	std::uniform_real_distribution pos_x_dist{-game_res.x / 2.0F, game_res.x / 2.0F};
	std::uniform_real_distribution pos_y_dist{-game_res.y / 2.0F, game_res.y / 2.0F};
	std::uniform_int_distribution<size_t> color_dist{0, 2};
	std::uniform_int_distribution<> shape_dist{shape_rect, shape_circle};

	// NOLINTNEXTLINE (*-pro-bounds-avoid-unchecked-container-access)
	while(color_counts_[0] < shapes_count_per_color || color_counts_[1] < shapes_count_per_color
		  || color_counts_[2] < shapes_count_per_color) { // NOLINT(*-pro-bounds-avoid-unchecked-container-access)
		// pick a random color that hasn't reached its quota yet
		size_t color_index = color_dist(rng);
		while(color_counts_[color_index] >= shapes_count_per_color) { // NOLINT(*-pro-bounds-constant-array-index,
																	  // *-pro-bounds-avoid-unchecked-container-access)
			color_index = color_dist(rng);
		}

		// just to be extra sure, this should never fail, index is between 0 and 2 inclusive, this does not add
		// any penalty on release builds, and it can help catch logic errors during development
		assert(color_index < colors.size());

		const auto &color = colors.at(color_index);
		color_counts_[color_index]++; // NOLINT(*-pro-bounds-constant-array-index, *-pro-bounds-avoid-unchecked-container-access)

		const auto pos_x = pos_x_dist(rng);
		const auto pos_y = pos_y_dist(rng);

		switch(shape_dist(rng)) {
		case shape_rect:
			create_rectangle(pos_x, pos_y, color, rng);
			break;
		case shape_circle:
			create_circle(pos_x, pos_y, color, rng);
			break;
		default:
			LGE_ERROR("invalid shape type generated: {}", shape_dist(rng));
			break;
		}
	}
}

auto layers::create_rectangle(float pos_x, float pos_y, const glm::vec4 &color, std::mt19937 &rng) -> void {
	static constexpr auto game_res = get_game_res();
	auto &world = get_world();

	std::uniform_real_distribution size_dist_w{rect_min_width, rect_max_width};
	std::uniform_real_distribution size_dist_h{rect_min_height, rect_max_height};
	std::uniform_real_distribution padding_dist{rect_min_border_padding, rect_max_border_padding};

	// generate random size
	const float width = size_dist_w(rng);
	const float height = size_dist_h(rng);

	const float half_width = width * 0.5F;
	const float half_height = height * 0.5F;

	// clamp position so the rect stays inside the game bounds
	if(pos_x - half_width < -game_res.x * 0.5F) {
		pos_x = (-game_res.x * 0.5F) + half_width + padding_dist(rng);
	}
	if(pos_x + half_width > game_res.x * 0.5F) {
		pos_x = (game_res.x * 0.5F) - half_width - padding_dist(rng);
	}
	if(pos_y - half_height < -game_res.y * 0.5F) {
		pos_y = (-game_res.y * 0.5F) + half_height + padding_dist(rng);
	}
	if(pos_y + half_height > game_res.y * 0.5F) {
		pos_y = (game_res.y * 0.5F) - half_height - padding_dist(rng);
	}

	// create rectangle entity
	const entt::entity rect_ent = world.create();

	auto &rect = world.emplace<lge::rect>(rect_ent);
	rect.size = {width, height};
	rect.fill_color = color;
	rect.border_color = shapes_border_color;
	rect.border_thickness = shapes_border_thickness;

	// placement is the center of the rect
	world.emplace<lge::placement>(rect_ent, pos_x, pos_y);
}

auto layers::create_circle(float pos_x, float pos_y, const glm::vec4 &color, std::mt19937 &rng) -> void {
	static constexpr auto game_res = get_game_res();
	auto &world = get_world();

	std::uniform_real_distribution padding_dist{rect_min_border_padding, rect_max_border_padding};
	std::uniform_real_distribution radius_dist{circle_min_radius, circle_max_radius};

	// generate random size and position
	const auto radius = radius_dist(rng);
	const auto width = radius * 2.0F;
	const auto height = radius * 2.0F;
	const auto half_width = width / 2.0F;
	const auto half_height = height / 2.0F;

	// adjust position if circle would go out of bounds, with random padding to avoid edge clustering
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
	const auto circle_ent = world.create();
	auto &circle = world.emplace<lge::circle>(circle_ent);
	circle.radius = radius;
	circle.fill_color = color;
	circle.border_color = shapes_border_color;
	circle.border_thickness = shapes_border_thickness;

	world.emplace<lge::placement>(circle_ent, pos_x, pos_y);
}
