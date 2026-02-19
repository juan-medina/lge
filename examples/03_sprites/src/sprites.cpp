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

	if(const auto err = resource_mgr.load_sprite_sheet(sprite_sheet_path).unwrap(sprite_sheet_); err) [[unlikely]] {
		return lge::error("failed to load sprite sheet", *err);
	}

	const auto sprite_entity = world.create();
	world.emplace<lge::sprite>(sprite_entity, sprite_sheet_, sprite_name);
	world.emplace<lge::placement>(sprite_entity, 0.0F, 0.0F);

	return true;
}

auto sprites::update(const float dt) -> lge::result<> {
	auto &world = get_world();
	auto &p = world.get<lge::placement>(world.view<lge::sprite>().front());
	p.rotation += 90.F * dt;
	return example::update(dt);
}

auto sprites::end() -> lge::result<> {
	auto &resource_mgr = get_resource_manager();
	if(const auto err = resource_mgr.unload_sprite_sheet(sprite_sheet_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unload sprite sheet", *err);
	}

	return example::end();
}