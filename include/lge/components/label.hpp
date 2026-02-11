// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <glm/ext/vector_float4.hpp>
#include <string>

namespace lge {

enum class vertical_alignment : std::uint8_t { top, center, bottom };

enum class horizontal_alignment : std::uint8_t { left, center, right };

struct label {
	std::string text;
	glm::vec4 color{1, 1, 1, 1};
	float size = 16.0F;
	vertical_alignment vertical_align = vertical_alignment::top;
	horizontal_alignment horizontal_align = horizontal_alignment::left;

	std::string previous_text;
	float previous_size = 16.0F;
	vertical_alignment previous_vertical_align = vertical_alignment::top;
	horizontal_alignment previous_horizontal_align = horizontal_alignment::left;
};

} // namespace lge