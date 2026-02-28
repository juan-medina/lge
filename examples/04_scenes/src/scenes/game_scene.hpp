// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/scene/scene.hpp>

#include "../events.hpp"

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>

namespace examples {

class game_scene: public lge::scene {
public:
	using scene::scene;
	[[nodiscard]] auto init() -> lge::result<>;
	[[nodiscard]] auto on_enter(game_type type) -> lge::result<>;

	[[nodiscard]] auto update(float dt) -> lge::result<> override;

private:
	entt::entity menu_message_ent_ = entt::null;
	static constexpr auto kb_message = "{#FFFF00}Backspace{#} to return to menu";
	static constexpr auto controller_message = "{#FFFF00}Controller Y{#} to return to menu";
};

} // namespace examples