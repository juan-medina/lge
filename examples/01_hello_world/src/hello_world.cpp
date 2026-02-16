// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "hello_world.hpp"

#include <lge/app/main.hpp>
#include <lge/components/hidden.hpp>
#include <lge/components/hierarchy.hpp>
#include <lge/components/label.hpp>
#include <lge/components/placement.hpp>
#include <lge/core/result.hpp>
#include <lge/systems/system.hpp>

#include "../../src/example.hpp"
#include "oscillation_system.hpp"

LGE_MAIN(hello_world);

auto hello_world::init() -> lge::result<> {
	if(const auto err = example::init().unwrap(); err) [[unlikely]] {
		return lge::error("error init the app", *err);
	}

	auto &input = get_input();
	input.bind(hide_hello_world_action,
			   {
				   .keys = {lge::input::key::one},
				   .buttons = {lge::input::button::right_face_down},
			   });

	input.bind(hide_world_action,
			   {
				   .keys = {lge::input::key::two},
				   .buttons = {lge::input::button::right_face_left},
			   });

	auto &world = get_world();

	center_ent_ = world.create();
	world.emplace<lge::placement>(center_ent_, 0, 0);
	world.emplace<oscillation_system::effect>(center_ent_,
											  oscillation_system::effect{
												  .scale = {.min = 1.0F, .max = 3.0F, .period = 8.0F},
												  .rotation = {.min = 0.0F, .max = 360.0F, .period = 4.0F},
											  });

	static constexpr auto gap_between_labels = 10.0F;

	const auto hello_text_ent = world.create();
	auto &hello_label = world.emplace<lge::label>(hello_text_ent, "Hello");
	hello_label.color = {1, 1, 0, 1};
	world.emplace<lge::placement>(
		hello_text_ent, 0, -gap_between_labels / 2, 0.0F, glm::vec2{1.F, 1.F}, lge::pivot::bottom_center);
	lge::attach(world, center_ent_, hello_text_ent);

	world_text_ent_ = world.create();
	auto &world_label = world.emplace<lge::label>(world_text_ent_, "World");
	world_label.color = {0, 1, 1, 1};
	world.emplace<lge::placement>(
		world_text_ent_, 0, gap_between_labels / 2, 0.0F, glm::vec2{1.F, 1.F}, lge::pivot::top_center);
	lge::attach(world, center_ent_, world_text_ent_);

	register_system<oscillation_system>(lge::phase::game_update);

	return true;
}

auto hello_world::update(const float dt) -> lge::result<> {
	auto &world = get_world();
	const auto &input = get_input();

	// toggle hidden on the center entity, this will hide both labels since they are children of the center entity
	if(input.get(hide_hello_world_action).pressed) {
		if(world.any_of<lge::hidden>(center_ent_)) {
			world.remove<lge::hidden>(center_ent_);
		} else {
			world.emplace<lge::hidden>(center_ent_);
		}
	}

	// toggle hidden on the world label, this will hide only the world label
	if(input.get(hide_world_action).pressed) {
		if(world.any_of<lge::hidden>(world_text_ent_)) {
			world.remove<lge::hidden>(world_text_ent_);
		} else {
			world.emplace<lge::hidden>(world_text_ent_);
		}
	}

	return example::update(dt);
}