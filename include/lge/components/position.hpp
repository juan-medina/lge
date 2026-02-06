// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <glm/glm.hpp>

namespace lge {

struct local_position {
	glm::vec2 value{0, 0};
	local_position() = default;
	local_position(const float x, const float y): value{x, y} {}
	local_position(const int x, const int y): value{x, y} {}
	explicit local_position(const glm::vec2 v): value{v} {}
};

struct global_position {
	glm::vec2 value{0, 0};
	global_position() = default;
	global_position(const float x, const float y): value{x, y} {}
	global_position(const int x, const int y): value{x, y} {}
	explicit global_position(const glm::vec2 v): value{v} {}
};

} // namespace lge