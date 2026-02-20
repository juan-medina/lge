// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "sprites.hpp"

#include <lge/app/main.hpp>
#include <lge/components/placement.hpp>
#include <lge/components/sprite_animation.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include "../../src/example.hpp"

#include <cassert>
#include <entt/entt.hpp>

LGE_MAIN(sprites);

auto sprites::init() -> lge::result<> {
	if(const auto err = example::init().unwrap(); err) [[unlikely]] {
		return lge::error("error init the app", *err);
	}

	auto &input = get_input();
	input.bind(left_action,
			   {
				   .keys = {lge::input::key::left, lge::input::key::a, lge::input::key::o},
				   .buttons = {lge::input::button::left_face_left},
			   });

	input.bind(right_action,
			   {
				   .keys = {lge::input::key::right, lge::input::key::d, lge::input::key::p},
				   .buttons = {lge::input::button::left_face_right},
			   });

	auto &world = get_world();
	auto &resource_mgr = get_resource_manager();

	if(const auto err = resource_mgr.load_animation_library(anim_path).unwrap(animation_library_); err) [[unlikely]] {
		return lge::error("failed to load animation library", *err);
	}

	sprite_ = world.create();
	world.emplace<lge::sprite_animation>(sprite_, animation_library_, idle_anim);
	auto &p = world.emplace<lge::placement>(sprite_, 0.0F, 0.0F);
	p.pivot = lge::pivot::bottom_center;

	return true;
}

auto sprites::update(const float dt) -> lge::result<> {
	const auto &input = get_input();

	const auto move_left = input.get(left_action).down;
	const auto move_right = input.get(right_action).down;

	auto &world = get_world();
	auto &anim = world.get<lge::sprite_animation>(sprite_);

	anim.name = (move_left || move_right) ? run_anim : idle_anim;
	if(move_left || move_right) {
		anim.flip_horizontal = move_left && !move_right;
	}

	return example::update(dt);
}

auto sprites::end() -> lge::result<> {
	auto &resource_mgr = get_resource_manager();

	if(const auto err = resource_mgr.unload_animation_library(animation_library_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unload animation library", *err);
	}

	return example::end();
}