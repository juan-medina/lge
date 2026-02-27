// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resources.hpp>

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
	static constexpr auto kb_message = "Space/Enter: throw dice, \nF5: debug draw, F11: fullscreen, Esc: exit";
	static constexpr auto controller_message = "A: throw dice, \nSTART: debug draw, SELECT: fullscreen, B: exit";

	static constexpr size_t throw_action = actions::total_base_actions + 0;

	[[nodiscard]] auto on_dice_roll_result(const dice_roll_result &result) -> lge::result<>;
};

} // namespace examples