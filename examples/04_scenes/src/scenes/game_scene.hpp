// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/scene/scene.hpp>

namespace examples {

class game_scene: public lge::scene {
public:
	using scene::scene;
	[[nodiscard]] auto init() -> lge::result<>;
	[[nodiscard]] auto on_enter() -> lge::result<>;

	[[nodiscard]] auto update(float dt) -> lge::result<> override;

	struct go_to_menu {};
};

} // namespace examples