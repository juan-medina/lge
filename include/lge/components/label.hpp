// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/colors.hpp>
#include <lge/interface/resource_manager.hpp>

#include <string>

namespace lge {

struct label {
	std::string text;
	color text_color = colors::white;
	float size = 17.0F;
	font_handle font = invalid_font;
};

} // namespace lge