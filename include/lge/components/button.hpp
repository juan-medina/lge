// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/colors.hpp>
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
	color hover_tint = color{200, 200, 200, 255};
	color pressed_tint = color{150, 150, 150, 255};
	color text_color = colors::white;
	float text_size = 17.0F;
	font_handle font = invalid_font;
};

} // namespace lge