// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <glm/ext/vector_float2.hpp>

namespace examples {

struct movement {
	glm::vec2 velocity{};
	float rotation_speed{};
	float delay{0.0F};
};

} // namespace examples
