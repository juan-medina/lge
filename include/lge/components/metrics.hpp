// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <glm/vec2.hpp>

namespace lge {

// local space size and pivot offset
struct metrics {
	glm::vec2 size;
	glm::vec2 pivot_offset;
};

} // namespace lge