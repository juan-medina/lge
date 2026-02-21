// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/colors.hpp>

#include <glm/ext/vector_float2.hpp>

namespace lge {

struct rect {
	glm::vec2 size;
	color border_color = colors::white;
	color fill_color = colors::transparent;
	float border_thickness = 1.0F;
};

struct circle {
	float radius;
	color border_color = colors::white;
	color fill_color = colors::transparent;
	float border_thickness = 1.0F;
};

} // namespace lge