// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include "glm/ext/vector_float2.hpp"

namespace lge {

// local space position, rotation, scale, and pivot
struct placement {
	placement(const float x,
			  const float y,
			  const float rot = 0.F,
			  const glm::vec2 scl = {1.F, 1.F},
			  const glm::vec2 pvt = {0.5F, 0.5F})
		: position{x, y}, rotation(rot), scale(scl), pivot(pvt) {}

	glm::vec2 position; // NOLINT(*-non-private-member-variables-in-classes)
	float rotation;		// NOLINT(*-non-private-member-variables-in-classes)
	glm::vec2 scale;	// NOLINT(*-non-private-member-variables-in-classes)
	glm::vec2 pivot;	// NOLINT(*-non-private-member-variables-in-classes)
};

namespace pivot {
	constexpr glm::vec2 top_left{0.F, 0.F};
	constexpr glm::vec2 top_center{0.5F, 0.F};
	constexpr glm::vec2 top_right{1.F, 0.F};
	constexpr glm::vec2 center_left{0.F, 0.5F};
	constexpr glm::vec2 center{0.5F, 0.5F};
	constexpr glm::vec2 center_right{1.F, 0.5F};
	constexpr glm::vec2 bottom_left{0.F, 1.F};
	constexpr glm::vec2 bottom_center{0.5F, 1.F};
	constexpr glm::vec2 bottom_right{1.F, 1.F};
} // namespace pivot

} // namespace lge