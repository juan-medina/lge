// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "sprites.hpp"

#include <lge/app/main.hpp>
#include <lge/components/placement.hpp>
#include <lge/components/sprite.hpp>
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

	auto &world = get_world();
	auto &resource_mgr = get_resource_manager();

	if(const auto err = resource_mgr.load_texture(sprite_path).unwrap(sprite_texture_); err) [[unlikely]] {
		return lge::error("failed to load sprite texture", *err);
	}

	const auto sprite_entity = world.create();
	world.emplace<lge::sprite>(sprite_entity, sprite_texture_);
	world.emplace<lge::placement>(sprite_entity, 0.0F, 0.0F);

	return true;
}

lge::result<> sprites::update(const float dt) {
	auto &world = get_world();
	auto &p = world.get<lge::placement>(world.view<lge::sprite>().front());
	p.rotation += 90.F * dt;
	return example::update(dt);
}

auto sprites::end() -> lge::result<> {
	auto &resource_mgr = get_resource_manager();
	if(const auto err = resource_mgr.unload_texture(sprite_texture_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unload sprite texture", *err);
	}

	return example::end();
}