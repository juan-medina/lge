// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/scene/scene.hpp>

#include "../events.hpp"

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>

namespace examples {

class menu_scene: public lge::scene {
public:
	using scene::scene;
	[[nodiscard]] auto init() -> lge::result<>;
	[[nodiscard]] auto on_enter() -> lge::result<>;

	[[nodiscard]] auto update(float dt) -> lge::result<> override;

private:
	game_type selected_game_{game_type::red};

	auto change_selection(game_type type) -> void;

	entt::entity menu_message_ent_ = entt::null;
	static constexpr auto kb_message = "Cursor left/right & enter to choose";
	static constexpr auto controller_message = "Controller left/right & A to choose";
};

} // namespace examples
