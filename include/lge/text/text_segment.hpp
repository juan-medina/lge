// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/colors.hpp>

#include <string>

namespace lge {
// =============================================================================
// Rich text segment
// =============================================================================

struct text_segment {
	std::string text;
	color segment_color;
};

} // namespace lge