// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/colors.hpp>
#include <lge/interface/resources.hpp>

#include <string>

namespace lge {

struct previous_label {
	std::string text;
	float size = 16.0F;
	font_handle font;
	color text_color = colors::white;
};

} // namespace lge