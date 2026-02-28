// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>

#include "../../src/actions.hpp"
#include "../../src/example.hpp"
#include "events.hpp"

#include <cstddef>

namespace examples {

class collisions: public example {
public:
	explicit collisions(): example(example_title, kb_message, controller_message, custom_clear_color) {}
	[[nodiscard]] auto update(float dt) -> lge::result<> override;

protected:
	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto end() -> lge::result<> override;

private:
	static constexpr auto custom_clear_color = lge::color::from_hex(0x1E303CFF);
	static constexpr auto example_title = "Collisions!";
	static constexpr auto kb_message = "{#FFFF00}Space/Enter{#}: throw dice, \n{#FFFF00}F5{#}: debug draw, "
									   "{#FFFF00}F11{#}: fullscreen, {#FFFF00}Esc{#}: exit";
	static constexpr auto controller_message = "{#FFFF00}A{#}: throw dice, \n{#FFFF00}START{#}: debug draw, "
											   "{#FFFF00}SELECT{#}: fullscreen, {#FFFF00}B{#}: exit";

	static constexpr size_t throw_action = actions::total_base_actions + 0;

	[[nodiscard]] auto on_dice_roll_result(const dice_roll_result &result) -> lge::result<>;
};

} // namespace examples