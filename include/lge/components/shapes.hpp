// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float4.hpp>

namespace lge {

struct rect {
	glm::vec2 size;
	glm::vec4 border_color{1, 1, 1, 1};
	glm::vec4 fill_color{0, 0, 0, 0};
	float border_thickness = 1.0F;

	glm::vec2 previous_size;
};

struct circle {
	float radius;
	glm::vec4 border_color{1, 1, 1, 1};
	glm::vec4 fill_color{0, 0, 0, 0};
	float border_thickness = 1.0F;

	float previous_radius;
};

} // namespace lge