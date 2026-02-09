// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include "glm/ext/vector_float2.hpp"

namespace lge {

// local space position, rotation, and scale
struct placement {
	placement(const float x, const float y, const float rot = 0.F, const glm::vec2 scl = {1.F, 1.F})
		: position{x, y}, rotation(rot), scale(scl) {}
	glm::vec2 position; // NOLINT(*-non-private-member-variables-in-classes)
	float rotation;		// NOLINT(*-non-private-member-variables-in-classes)
	glm::vec2 scale;	// NOLINT(*-non-private-member-variables-in-classes)
};

} // namespace lge