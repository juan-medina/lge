// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/text/text_segment.hpp>

#include <vector>

namespace lge {

struct rich_segments {
	std::vector<text_segment> segments;
};

} // namespace lge