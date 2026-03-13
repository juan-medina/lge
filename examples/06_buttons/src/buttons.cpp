// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "buttons.hpp"

#include <lge/app/main.hpp>
#include <lge/components/button.hpp>
#include <lge/components/hidden.hpp>
#include <lge/components/hierarchy.hpp>
#include <lge/components/label.hpp>
#include <lge/components/panel.hpp>
#include <lge/components/placement.hpp>
#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>
#include <lge/events/button_clicked.hpp>
#include <lge/interface/input.hpp>
#include <lge/interface/resource_manager.hpp>

#include "../../src/example.hpp"

#include <entt/core/hashed_string.hpp>
#include <string_view>

LGE_MAIN(examples::buttons);

namespace examples {

using entt::literals::operator""_hs;

auto buttons::init() -> lge::result<> {
	if(const auto err = example::init().unwrap(); err) [[unlikely]] {
		return lge::error("error init the app", *err);
	}

	if(const auto err = ctx.resources.load_sprite_sheet(ui_sheet_path).unwrap(ui_sheet_); err) [[unlikely]] {
		return lge::error("failed to load ui sprite sheet", *err);
	}

	if(const auto err = ctx.resources.load_sprite_sheet(input_sheet_path).unwrap(input_sheet_); err) [[unlikely]] {
		return lge::error("failed to load input sprite sheet", *err);
	}

	// -------------------------------------------------------------------------
	// Popup button — always visible
	// -------------------------------------------------------------------------
	popup_button_ = ctx.world.create();
	ctx.world.emplace<lge::button>(popup_button_,
								   lge::button{
									   .sheet = ui_sheet_,
									   .frame = "tile_0003.png"_hs,
									   .size = glm::vec2{80.0F, 30.0F},
									   .border = 15.0F,
									   .text = "Popup",
									   .normal_tint = lge::color::from_hex(0x5078C8FF),
									   .hover_tint = lge::color::from_hex(0x6EA0F0FF),
									   .pressed_tint = lge::color::from_hex(0x32508CFF),
									   .text_color = lge::colors::white,
									   .controller_button = lge::input::button::right_face_up,
									   .overlay_sheet = input_sheet_,
									   .overlay_frame = "tile_0007.png"_hs,
								   });
	ctx.world.emplace<lge::placement>(popup_button_, 0.0F, 0.0F);

	// -------------------------------------------------------------------------
	// Result label — shows what was last pressed, starts empty
	// -------------------------------------------------------------------------
	result_label_ = ctx.world.create();
	ctx.world.emplace<lge::label>(result_label_, "", lge::colors::white, 20.0F);
	ctx.world.emplace<lge::placement>(result_label_, 0.0F, -50.0F);
	ctx.world.emplace<lge::hidden>(result_label_);

	// -------------------------------------------------------------------------
	// Popup panel — hidden until popup_button_ is clicked
	// -------------------------------------------------------------------------
	popup_panel_ = ctx.world.create();
	ctx.world.emplace<lge::panel>(popup_panel_, ui_sheet_, "tile_0017.png"_hs, glm::vec2{200.0F, 90.0F}, 15.0F);
	ctx.world.emplace<lge::placement>(popup_panel_, 0.0F, 0.0F);
	ctx.world.emplace<lge::hidden>(popup_panel_);

	const auto panel_label = ctx.world.create();
	ctx.world.emplace<lge::label>(panel_label, "Are you sure?", lge::colors::white, 20.0F);
	ctx.world.emplace<lge::placement>(panel_label, 0.0F, -20.0F);
	lge::attach(ctx.world, popup_panel_, panel_label);

	ok_button_ = ctx.world.create();
	ctx.world.emplace<lge::button>(ok_button_,
								   lge::button{
									   .sheet = ui_sheet_,
									   .frame = "tile_0003.png"_hs,
									   .size = glm::vec2{70.0F, 25.0F},
									   .border = 15.0F,
									   .text = "Ok",
									   .normal_tint = lge::color::from_hex(0x64B464FF),
									   .hover_tint = lge::color::from_hex(0x8CDC8CFF),
									   .pressed_tint = lge::color::from_hex(0x3C783CFF),
									   .text_color = lge::colors::white,
									   .controller_button = lge::input::button::right_face_down,
									   .overlay_sheet = input_sheet_,
									   .overlay_frame = "tile_0004.png"_hs,
								   });
	ctx.world.emplace<lge::placement>(ok_button_, -45.0F, 20.0F);
	lge::attach(ctx.world, popup_panel_, ok_button_);

	cancel_button_ = ctx.world.create();
	ctx.world.emplace<lge::button>(cancel_button_,
								   lge::button{
									   .sheet = ui_sheet_,
									   .frame = "tile_0003.png"_hs,
									   .size = glm::vec2{70.0F, 25.0F},
									   .border = 15.0F,
									   .text = "Cancel",
									   .normal_tint = lge::color::from_hex(0xB43C3CFF),
									   .hover_tint = lge::color::from_hex(0xDC6464FF),
									   .pressed_tint = lge::color::from_hex(0x781E1EFF),
									   .text_color = lge::colors::white,
									   .controller_button = lge::input::button::right_face_left,
									   .overlay_sheet = input_sheet_,
									   .overlay_frame = "tile_0006.png"_hs,
								   });
	ctx.world.emplace<lge::placement>(cancel_button_, 45.0F, 20.0F);
	lge::attach(ctx.world, popup_panel_, cancel_button_);

	// -------------------------------------------------------------------------
	// Button event handler
	// -------------------------------------------------------------------------
	btn_sub_ = ctx.events.subscribe<lge::button_clicked>([this](const lge::button_clicked &e) -> lge::result<> {
		if(e.entity == popup_button_) {
			show_popup();
		} else if(e.entity == ok_button_) {
			hide_popup("{#00FF00}Ok{#} was pressed");
		} else if(e.entity == cancel_button_) {
			hide_popup("{#FF4444}Cancel{#} was pressed");
		}
		return true;
	});

	return true;
}

auto buttons::update(const float dt) -> lge::result<> {
	return example::update(dt);
}

auto buttons::end() -> lge::result<> {
	if(const auto err = ctx.events.unsubscribe(btn_sub_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unsubscribe button_clicked event", *err);
	}

	if(const auto err = ctx.resources.unload_sprite_sheet(ui_sheet_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unload ui sprite sheet", *err);
	}

	if(const auto err = ctx.resources.unload_sprite_sheet(input_sheet_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unload input sprite sheet", *err);
	}

	return example::end();
}

auto buttons::show_popup() -> void {
	ctx.world.remove<lge::hidden>(popup_panel_);
	ctx.world.emplace_or_replace<lge::hidden>(popup_button_);
	ctx.world.emplace_or_replace<lge::hidden>(result_label_);
}

auto buttons::hide_popup(const std::string_view result) -> void {
	ctx.world.emplace_or_replace<lge::hidden>(popup_panel_);
	ctx.world.remove<lge::hidden>(popup_button_);
	ctx.world.remove<lge::hidden>(result_label_);
	ctx.world.get<lge::label>(result_label_).text = std::string(result);
}

} // namespace examples