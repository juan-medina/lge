// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/dispatcher/subscription.hpp>
#include <lge/interface/resources.hpp>

#include "../../src/example.hpp"

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <string_view>

namespace examples {

class buttons: public example {
public:
	explicit buttons(): example(example_title, kb_message, controller_message) {}

	[[nodiscard]] auto update(float dt) -> lge::result<> override;

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
	static constexpr auto input_sheet_path = "resources/game/sprites/input.json";

	lge::sprite_sheet_handle ui_sheet_{lge::invalid_sprite_sheet};
	lge::sprite_sheet_handle input_sheet_{lge::invalid_sprite_sheet};

	entt::entity popup_button_{entt::null};
	entt::entity popup_panel_{entt::null};
	entt::entity result_label_{entt::null};
	entt::entity ok_button_{entt::null};
	entt::entity cancel_button_{entt::null};

	lge::subscription btn_sub_{};

	auto show_popup() -> void;
	auto hide_popup(std::string_view result) -> void;
};

} // namespace examples