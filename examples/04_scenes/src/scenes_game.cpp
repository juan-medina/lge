// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "scenes_game.hpp"

#include <lge/app/main.hpp>
#include <lge/core/result.hpp>
#include <lge/scene/scene_manager.hpp>

#include "../../src/example.hpp"
#include "events.hpp"
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

	go_to_game_sub_ = ctx.events.subscribe<go_to_game>([this](const auto &evt) -> lge::result<> {
		if(const auto err = scenes.transition_activate<game_scene>(evt.type).unwrap(); err) [[unlikely]] {
			return lge::error("failed to set active scene to game scene", *err);
		}
		return true;
	});

	go_to_menu_sub_ = ctx.events.subscribe<go_to_menu>([this](const auto &) -> lge::result<> {
		if(const auto err = scenes.transition_activate<menu_scene>().unwrap(); err) [[unlikely]] {
			return lge::error("failed to set active scene to menu scene", *err);
		}
		return true;
	});

	return true;
}

auto scenes_game::update(const float dt) -> lge::result<> {
	return example::update(dt);
}

auto scenes_game::end() -> lge::result<> {
	if(const auto err = ctx.events.unsubscribe(go_to_game_sub_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unsubscribe go_to_game event", *err);
	}

	if(const auto err = ctx.events.unsubscribe(go_to_menu_sub_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unsubscribe go_to_menu event", *err);
	}

	return example::end();
}

} // namespace examples
