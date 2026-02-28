// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "lge/internal/text/rich_text.hpp"

#include <lge/core/colors.hpp>

#include <charconv>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace lge {

namespace {

// =============================================================================
// Tag parsing helpers
// =============================================================================

constexpr auto tag_open = '{';
constexpr auto tag_close = '}';
constexpr auto tag_prefix = '#';

constexpr auto hex_rgb_len = 6;
constexpr auto hex_rgba_len = 8;
constexpr auto tag_rgb_len = 4;	  // {#RRGGBB}   → inner = '#' + 6 hex = 7, full = 9
constexpr auto tag_rgba_len = 5;  // {#RRGGBBAA} → inner = '#' + 8 hex = 9, full = 11
constexpr auto tag_reset_len = 3; // {#}

// Returns true and advances pos past the closing '}' if a valid tag starts at pos.
// On success, out_color is set to the parsed color (or default_color for {#}).
// On failure, pos is unchanged and the caller must treat '{' as literal text.
[[nodiscard]] auto try_parse_tag(std::string_view text,
								 std::size_t pos,
								 const color &default_color,
								 color &out_color,
								 std::size_t &out_end) noexcept -> bool {
	// Minimum tag: {#}  = 3 chars
	if(pos + tag_reset_len > text.size()) [[unlikely]] {
		return false;
	}

	if(text[pos] != tag_open || text[pos + 1] != tag_prefix) [[likely]] {
		return false;
	}

	const auto close = text.find(tag_close, pos + 2);
	if(close == std::string_view::npos) [[unlikely]] {
		return false;
	}

	// inner is the content between '{' and '}', starting after '#'
	const auto hex = text.substr(pos + 2, close - pos - 2);

	if(hex.empty()) {
		// {#} — reset to default
		out_color = default_color;
		out_end = close + 1;
		return true;
	}

	if(hex.size() != hex_rgb_len && hex.size() != hex_rgba_len) [[unlikely]] {
		// wrong length — treat as literal
		return false;
	}

	// Parse hex digits
	uint32_t value = 0;
	const auto [ptr, ec] = std::from_chars(hex.data(), hex.data() + hex.size(), value, 16);
	if(ec != std::errc{} || ptr != hex.data() + hex.size()) [[unlikely]] {
		return false;
	}

	if(hex.size() == hex_rgb_len) {
		// RRGGBB — shift left 8 bits and fill alpha = 0xFF
		out_color = color::from_hex((value << 8) | 0xFF);
	} else {
		// RRGGBBAA
		out_color = color::from_hex(value);
	}

	out_end = close + 1;
	return true;
}

} // namespace

// =============================================================================
// Public API
// =============================================================================

auto parse_rich_text(const std::string_view text, const color &default_color) -> std::vector<text_segment> {
	std::vector<text_segment> segments;

	if(text.empty()) [[unlikely]] {
		return segments;
	}

	auto current_color = default_color;
	std::string current_text;
	current_text.reserve(text.size());

	std::size_t i = 0;
	while(i < text.size()) {
		if(text[i] == tag_open) [[unlikely]] {
			color next_color{};
			std::size_t tag_end = 0;

			if(try_parse_tag(text, i, default_color, next_color, tag_end)) {
				// flush current segment before switching color
				if(!current_text.empty()) {
					segments.push_back({std::move(current_text), current_color});
					current_text.clear();
					current_text.reserve(text.size() - tag_end);
				}
				current_color = next_color;
				i = tag_end;
				continue;
			}
		}

		current_text += text[i];
		++i;
	}

	if(!current_text.empty()) {
		segments.push_back({std::move(current_text), current_color});
	}

	return segments;
}

auto strip_rich_tags(const std::string_view text) -> std::string {
	std::string result;
	result.reserve(text.size());

	std::size_t i = 0;
	while(i < text.size()) {
		if(text[i] == tag_open) [[unlikely]] {
			color dummy{};
			std::size_t tag_end = 0;
			if(try_parse_tag(text, i, colors::white, dummy, tag_end)) {
				i = tag_end;
				continue;
			}
		}
		result += text[i];
		++i;
	}

	return result;
}

auto has_rich_tags(const std::string_view text) noexcept -> bool {
	const auto pos = text.find('{');
	if(pos == std::string_view::npos) {
		return false;
	}
	return pos + 1 < text.size() && text[pos + 1] == tag_prefix;
}

} // namespace lge