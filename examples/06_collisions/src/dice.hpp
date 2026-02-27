// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <glm/ext/vector_float2.hpp>

namespace examples {

struct dice {
	glm::vec2 velocity{};
	float rotation_speed{};
	float delay{0.0F};
	glm::vec2 half_size{};
	int value{1};
	bool settled{false};
};

} // namespace examples