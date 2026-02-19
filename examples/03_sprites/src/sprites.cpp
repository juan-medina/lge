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
	input.bind(change_anim_action,
			   {
				   .keys = {lge::input::key::space},
				   .buttons = {lge::input::button::right_face_down},
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
	if(input.get(change_anim_action).pressed) {
		if(anim_state_ == animation_state::idle) {
			anim_state_ = animation_state::run;
		} else {
			anim_state_ = animation_state::idle;
		}

		auto &world = get_world();
		auto &anim = world.get<lge::sprite_animation>(sprite_);
		lge::play(anim, anim_state_ == animation_state::idle ? idle_anim : run_anim);
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