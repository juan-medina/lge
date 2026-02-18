// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <glm/ext/matrix_float3x3.hpp>

namespace lge {

// full 2D affine transform
struct transform {
	glm::mat3 world;
	glm::mat3 attachment;
};

} // namespace lge