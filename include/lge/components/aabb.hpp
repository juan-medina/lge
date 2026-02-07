// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <glm/glm.hpp>

namespace lge {

struct local_aabb {
	glm::vec2 min{};
	glm::vec2 max{};
};

struct global_aabb {
	glm::vec2 min{};
	glm::vec2 max{};
};

} // namespace lge