// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/dispatcher/subscription.hpp>

#include "../../src/example.hpp"

namespace examples {

class scenes_game: public example {
public:
	explicit scenes_game(): example(example_title, kb_message, controller_message) {}
	[[nodiscard]] auto update(float dt) -> lge::result<> override;

protected:
	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto end() -> lge::result<> override;

private:
	lge::subscription go_to_game_sub_;
	lge::subscription go_to_menu_sub_;

	static constexpr auto example_title = "scenes_game!";
	static constexpr auto kb_message =
		"{#FFFF00}F5{#} debug draw, {#FFFF00}F11{#}: toggle fullscreen, {#FFFF00}Esc{#}: exit";
	static constexpr auto controller_message =
		"{#FFFF00}START{#}: debug draw, {#FFFF00}SELECT{#}: toggle fullscreen, {#FFFF00}B{#}: exit";
};

} // namespace examples
