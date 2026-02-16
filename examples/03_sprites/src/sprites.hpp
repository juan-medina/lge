// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>

#include "../../src/example.hpp"

#include <entity/entity.hpp>
#include <entity/fwd.hpp>
#include <random>

class sprites: public example {
public:
	explicit sprites(): example(example_title, kb_message, controller_message) {}

protected:
	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto end() -> lge::result<> override;

private:
	static constexpr auto example_title = "Sprites!";
	static constexpr auto kb_message = "draws a single sprite in the center";
	static constexpr auto controller_message = "draws a single sprite in the center";
	static constexpr auto sprite_path = "resources/game/sprites/logo-sm.png";

	lge::texture_handle sprite_texture_{lge::invalid_texture};
};
