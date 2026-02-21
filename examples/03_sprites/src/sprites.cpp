// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "sprites.hpp"

#include <lge/app/main.hpp>
#include <lge/components/order.hpp>
#include <lge/components/placement.hpp>
#include <lge/components/shapes.hpp>
#include <lge/components/sprite.hpp>
#include <lge/components/sprite_animation.hpp>
#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>
#include <lge/interface/resources.hpp>

#include "../../src/example.hpp"

#include <cassert>
#include <entt/entt.hpp>

LGE_MAIN(examples::sprites);

namespace examples {

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

	if(const auto err = resource_mgr.load_sprite_sheet(bg_path).unwrap(bg_sheet_); err) [[unlikely]] {
		return lge::error("failed to load background sprite sheet", *err);
	}

	constexpr auto dr = get_game_res();

	// a band across the center of the screen as background of all layers
	const auto center_band = world.create();
	world.emplace<lge::rect>(center_band, glm::vec2{dr.x * 2, dr.y / 2}, lge::colors::transparent, bands_color);
	world.emplace<lge::placement>(center_band, 0.0F, 0.0F);

	lge::sprite_sheet_frame frame{};
	if(const auto err = resource_mgr.get_sprite_sheet_frame(bg_sheet_, bg[0]).unwrap(frame); err) [[unlikely]] {
		return lge::error("failed to get background sprite sheet frame", *err);
	}
	bg_size_ = frame.source_size;

	int layer_num = 0;
	for(const auto bg_id: bg) {
		const float factor = factors[layer_num];
		const auto layer = world.create();
		world.emplace<lge::sprite>(layer, bg_sheet_, bg_id);
		world.emplace<lge::placement>(layer, 0.0F, 0.0F);
		world.emplace<lge::order>(layer, layer_num);
		world.emplace<parallax>(layer, factor);

		const auto layer_left = world.create();
		world.emplace<lge::sprite>(layer_left, bg_sheet_, bg_id);
		world.emplace<lge::placement>(layer_left, -bg_size_.x, 0.0F);
		world.emplace<lge::order>(layer_left, layer_num);
		world.emplace<parallax>(layer_left, factor);

		const auto layer_right = world.create();
		world.emplace<lge::sprite>(layer_right, bg_sheet_, bg_id);
		world.emplace<lge::placement>(layer_right, bg_size_.x, 0.0F);
		world.emplace<lge::order>(layer_right, layer_num);
		world.emplace<parallax>(layer_right, factor);

		layer_num++;
	}

	if(const auto err = resource_mgr.load_animation_library(anim_path).unwrap(animation_library_); err) [[unlikely]] {
		return lge::error("failed to load animation library", *err);
	}

	sprite_ = world.create();
	world.emplace<lge::sprite_animation>(sprite_, animation_library_, idle_anim);
	auto &p = world.emplace<lge::placement>(sprite_, 0.0F, character_y);
	p.pivot = lge::pivot::bottom_center;
	world.emplace<lge::order>(sprite_, 1);

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
		const auto dir = move_left ? -1.0F : 1.0F;

		for(const auto view = world.view<lge::placement, parallax>(); const auto ent: view) {
			auto &p = view.get<lge::placement>(ent);
			auto &[factor] = view.get<parallax>(ent);

			// Move layer opposite to player movement
			p.position.x += -dir * factor * run_speed * dt;

			// Wrap around
			if(p.position.x < -bg_size_.x * 1.5F) {
				p.position.x += bg_size_.x * 3.0F;
			} else if(p.position.x > bg_size_.x * 1.5F) {
				p.position.x -= bg_size_.x * 3.0F;
			}
		}
	}

	return example::update(dt);
}

auto sprites::end() -> lge::result<> {
	auto &resource_mgr = get_resource_manager();

	if(const auto err = resource_mgr.unload_animation_library(animation_library_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unload animation library", *err);
	}

	if(const auto err = resource_mgr.unload_sprite_sheet(bg_sheet_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unload background sprite sheet", *err);
	}

	return example::end();
}

} // namespace examples