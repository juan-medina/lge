// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <glm/vec2.hpp>

namespace lge {

// local space position, rotation, and scale
struct placement {
	glm::vec2 position;
	float rotation;
	glm::vec2 scale;
};

} // namespace lge