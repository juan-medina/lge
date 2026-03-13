// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/interface/resources.hpp>

#include "../../src/example.hpp"

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>

namespace examples {

class buttons: public example {
public:
	explicit buttons(): example(example_title, kb_message, controller_message) {}

protected:
	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto end() -> lge::result<> override;

private:
	static constexpr auto example_title = "Buttons!";
	static constexpr auto kb_message =
		"Press {#FFFF00}F5{#}: debug draw, {#FFFF00}F11{#}: toggle fullscreen, {#FFFF00}Esc{#}: exit";
	static constexpr auto controller_message =
		"Press {#FFFF00}START{#}: debug draw, {#FFFF00}SELECT{#}: toggle fullscreen, {#FFFF00}B{#}: exit";

	static constexpr auto ui_sheet_path = "resources/game/sprites/ui.json";

	lge::sprite_sheet_handle ui_sheet_{lge::invalid_sprite_sheet};
};

} // namespace examples
