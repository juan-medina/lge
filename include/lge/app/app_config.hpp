// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float4.hpp>
#include <string>

namespace lge {

struct app_config {
	glm::vec2 design_resolution{640, 360};
	glm::vec4 clear_color{0, 0, 0, 1};
	std::string window_title{"LGE Game"};
};

} // namespace lge
