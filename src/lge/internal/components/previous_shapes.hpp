// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float4.hpp>

namespace lge {

struct previous_rect {
	glm::vec2 size;
};

struct previous_circle {
	float radius;
};

} // namespace lge