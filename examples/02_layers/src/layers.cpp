// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "layers.hpp"

#include <lge/app/main.hpp>
#include <lge/components/hierarchy.hpp>
#include <lge/components/label.hpp>
#include <lge/components/order.hpp>
#include <lge/components/placement.hpp>
#include <lge/components/shapes.hpp>
#include <lge/core/result.hpp>

#include "../../src/example.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <entity/fwd.hpp>
#include <entt/entt.hpp>
#include <glm/ext/vector_float4.hpp>
#include <random>

LGE_MAIN(layers);

auto layers::init() -> lge::result<> {
	if(const auto err = example::init().unwrap(); err) [[unlikely]] {
		return lge::error("error init the app", *err);
	}

	auto &world = get_world();
	static constexpr auto game_res = get_game_res();

	top_text_entity_ = world.create();
	auto &message_label = world.emplace<lge::label>(top_text_entity_, "");
	message_label.size = 12;
	world.emplace<lge::placement>(
		top_text_entity_, 0.0F, -game_res.y / 2, 0.0F, glm::vec2{1.F, 1.F}, lge::pivot::top_center);

	auto &input = get_input();
	input.bind(change_top_action,
			   {
				   .keys = {lge::input::key::one},
				   .buttons = {lge::input::button::right_face_down},
			   });

	create_random_shapes();
	change_top_color(top_color::none);

	return true;
}

auto layers::update(const float dt) -> lge::result<> {
	const auto &input = get_input();

	if(input.get(change_top_action).pressed) {
		switch(top_color_) {
		case top_color::none:
			change_top_color(top_color::red);
			break;
		case top_color::red:
			change_top_color(top_color::green);
			break;
		case top_color::green:
			change_top_color(top_color::blue);
			break;
		case top_color::blue:
			change_top_color(top_color::none);
			break;
		}
	}

	return example::update(dt);
}

auto layers::change_top_color(const top_color new_top_color) -> void {
	top_color_ = new_top_color;

	glm::vec4 color;
	std::string txt = "top color: ";

	switch(top_color_) {
	case top_color::none:
		txt += "none";
		color = glm::vec4{1, 1, 1, 1};
		break;
	case top_color::red:
		txt += "red";
		color = glm::vec4{1, 0, 0, 1};
		break;
	case top_color::green:
		txt += "green";
		color = glm::vec4{0, 1, 0, 1};
		break;
	case top_color::blue:
		txt += "blue";
		color = glm::vec4{0, 0, 1, 1};
		break;
	}

	auto &world = get_world();

	auto &lbl = world.get<lge::label>(top_text_entity_);
	lbl.text = txt;
	lbl.color = color;

	world.get<lge::order>(red_root_).layer = (top_color_ == top_color::red) ? 1 : 0;
	world.get<lge::order>(green_root_).layer = (top_color_ == top_color::green) ? 1 : 0;
	world.get<lge::order>(blue_root_).layer = (top_color_ == top_color::blue) ? 1 : 0;
}

auto layers::create_root() -> entt::entity {
	auto &world = get_world();
	const auto root_entity = world.create();
	world.emplace<lge::order>(root_entity);
	world.emplace<lge::placement>(root_entity, 0.F, 0.F); // center of the screen
	return root_entity;
}

auto layers::create_random_shapes() -> void {
	std::random_device rd;
	std::mt19937 rng{rd()};

	auto &world = get_world();
	static constexpr auto game_res = get_game_res();

	red_root_ = create_root();
	green_root_ = create_root();
	blue_root_ = create_root();

	std::uniform_real_distribution pos_x_dist{-game_res.x / 2.0F, game_res.x / 2.0F};
	std::uniform_real_distribution pos_y_dist{(-game_res.y / 2.0F) + text_area_margin,
											  (game_res.y / 2.0F) - text_area_margin};
	std::uniform_int_distribution<size_t> color_dist{0, 2};
	std::uniform_int_distribution shape_dist{shape_rect, shape_circle};

	while(color_counts_[0] < shapes_count_per_color || color_counts_[1] < shapes_count_per_color
		  || color_counts_[2] < shapes_count_per_color) {
		// pick a random color that hasn't reached its quota yet
		size_t color_index = color_dist(rng);
		while(color_counts_[color_index] >= shapes_count_per_color) {
			color_index = color_dist(rng);
		}

		// just to be extra sure, this should never fail, index is between 0 and 2 inclusive, this does not add
		// any penalty on release builds, and it can help catch logic errors during development
		assert(color_index < colors.size());

		const auto &color = colors.at(color_index);
		color_counts_[color_index]++;

		const auto pos_x = pos_x_dist(rng);
		const auto pos_y = pos_y_dist(rng);

		entt::entity created_entity = entt::null;
		switch(shape_dist(rng)) {
		case shape_rect:
			created_entity = create_rectangle(pos_x, pos_y, color, rng);
			break;
		case shape_circle:
			created_entity = create_circle(pos_x, pos_y, color, rng);
			break;
		default:
			break;
		}
		entt::entity parent_entity = entt::null;
		switch(color_index) {
		case 0:
			parent_entity = red_root_;
			break;
		case 1:
			parent_entity = green_root_;
			break;
		case 2:
			parent_entity = blue_root_;
			break;
		default:
			break;
		}

		lge::attach(world, parent_entity, created_entity);
	}
}

auto layers::create_rectangle(float pos_x, float pos_y, const glm::vec4 &color, std::mt19937 &rng) -> entt::entity {
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
	if(pos_y - half_height < (-game_res.y * 0.5F) + text_area_margin) {
		pos_y = (-game_res.y * 0.5F) + text_area_margin + half_height + padding_dist(rng);
	}
	if(pos_y + half_height > (game_res.y * 0.5F) - text_area_margin) {
		pos_y = (game_res.y * 0.5F) - text_area_margin - half_height - padding_dist(rng);
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

	return rect_ent;
}

auto layers::create_circle(float pos_x, float pos_y, const glm::vec4 &color, std::mt19937 &rng) -> entt::entity {
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
	if(pos_y - half_height < (-game_res.y / 2.0F) + text_area_margin) {
		const auto padding = padding_dist(rng);
		pos_y = (-game_res.y / 2.0F) + text_area_margin + half_height + padding;
	}
	if(pos_y + half_height > (game_res.y / 2.0F) - text_area_margin) {
		const auto padding = padding_dist(rng);
		pos_y = (game_res.y / 2.0F) - text_area_margin - half_height - padding;
	}

	// create rectangle entity with calculated dimensions and position
	const auto circle_ent = world.create();
	auto &circle = world.emplace<lge::circle>(circle_ent);
	circle.radius = radius;
	circle.fill_color = color;
	circle.border_color = shapes_border_color;
	circle.border_thickness = shapes_border_thickness;

	world.emplace<lge::placement>(circle_ent, pos_x, pos_y);
	return circle_ent;
}
