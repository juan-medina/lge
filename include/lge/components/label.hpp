// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/interface/resource_manager.hpp>

#include <glm/ext/vector_float4.hpp>
#include <string>

namespace lge {

struct label {
	std::string text;
	glm::vec4 color{1, 1, 1, 1};
	float size = 16.0F;
	font_id font{empty_resource};
};

} // namespace lge