// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/colors.hpp>

#include <glm/ext/vector_float2.hpp>
#include <string>

namespace lge {

struct app_config {
	glm::vec2 design_resolution{640, 360};
	color clear_color{colors::black};
	std::string window_title{"LGE Game"};
};

} // namespace lge
