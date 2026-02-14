// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <glm/ext/vector_float2.hpp>

namespace lge {

// world space bounds of the entity, calculated from the transform and metrics
struct bounds {
	glm::vec2 p0;
	glm::vec2 p1;
	glm::vec2 p2;
	glm::vec2 p3;
};

} // namespace lge
