// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include "../../src/example.hpp"

class sprites: public example {
public:
	explicit sprites(): example(example_title, kb_message, controller_message) {}
	[[nodiscard]] auto update(float dt) -> lge::result<> override;

protected:
	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto end() -> lge::result<> override;

private:
	static constexpr auto example_title = "Sprites!";
	static constexpr auto kb_message = "draws a single sprite in the center";
	static constexpr auto controller_message = "draws a single sprite in the center";

	static constexpr auto sprite_sheet_path = "resources/game/sprites/sprites.json";
	static constexpr auto sprite_name = "logo-sm.png";

	lge::sprite_sheet_handle sprite_sheet_{lge::invalid_sprite_sheet};
};