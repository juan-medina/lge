// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/colors.hpp>
#include <lge/text/text_segment.hpp>

#include <string>
#include <string_view>
#include <vector>

namespace lge {

// =============================================================================
// Rich text parsing
//
// Supported tags embedded in a string:
//   {#RRGGBB}   — set segment color (full opacity)
//   {#RRGGBBAA} — set segment color with explicit alpha
//   {#}         — reset to default_color
//
// Malformed tags (wrong length, invalid hex) are treated as literal text.
// There is no nesting — {#} always resets to default_color, not a previous color.
// =============================================================================

[[nodiscard]] auto parse_rich_text(std::string_view text, const color &default_color) -> std::vector<text_segment>;

[[nodiscard]] auto strip_rich_tags(std::string_view text) -> std::string;

[[nodiscard]] auto has_rich_tags(std::string_view text) noexcept -> bool;

} // namespace lge