// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/colors.hpp>
#include <lge/interface/input.hpp>
#include <lge/interface/resources.hpp>

#include <entt/core/fwd.hpp>
#include <entt/core/hashed_string.hpp>
#include <glm/ext/vector_float2.hpp>
#include <string>

namespace lge {

using entt::literals::operator""_hs;

struct button {
	sprite_sheet_handle sheet;
	entt::hashed_string frame = ""_hs;
	glm::vec2 size{};
	float border{0.0F};
	std::string text;
	color normal_tint = colors::white;
	color hover_tint = colors::gray_80;
	color pressed_tint = colors::gray_40;
	color text_color = colors::white;
	float text_size = 17.0F;
	font_handle font = invalid_font;
	input::button controller_button = input::button::unknown;
	sprite_sheet_handle overlay_sheet = invalid_sprite_sheet;
	entt::hashed_string overlay_frame = ""_hs;
};

} // namespace lge