// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "scenes_game.hpp"

#include <lge/app/main.hpp>
#include <lge/core/result.hpp>
#include <lge/scene/scene_manager.hpp>

#include "../../src/example.hpp"
#include "scenes/game_scene.hpp"
#include "scenes/menu_scene.hpp"

LGE_MAIN(examples::scenes_game);

namespace examples {

auto scenes_game::init() -> lge::result<> {
	if(const auto err = example::init().unwrap(); err) [[unlikely]] {
		return lge::error("error init the app", *err);
	}

	if(const auto err = scenes.add<menu_scene>().unwrap(); err) [[unlikely]] {
		return lge::error("failed to register menu scene", *err);
	}
	if(const auto err = scenes.add<game_scene>().unwrap(); err) [[unlikely]] {
		return lge::error("failed to register menu scene", *err);
	}

	if(const auto err = scenes.activate<menu_scene>().unwrap(); err) [[unlikely]] {
		return lge::error("failed to set active scene to menu scene", *err);
	}

	ctx.events.on<menu_scene::go_to_game>([this](const auto &) -> lge::result<> {
		if(const auto err = scenes.activate<game_scene>().unwrap(); err) [[unlikely]] {
			return lge::error("failed to set active scene to game scene", *err);
		}
		return true;
	});

	return true;
}

auto scenes_game::update(const float dt) -> lge::result<> {
	return example::update(dt);
}

auto scenes_game::end() -> lge::result<> {
	return example::end();
}

} // namespace examples
