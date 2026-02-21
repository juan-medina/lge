#pragma once

#include <cstdint>

namespace lge {

// =============================================================================
// Color
// =============================================================================

struct color {
	uint8_t r{0};
	uint8_t g{0};
	uint8_t b{0};
	uint8_t a{255};

	[[nodiscard]] static constexpr auto from_hex(uint32_t rgba) noexcept -> color {
		return {
			static_cast<uint8_t>((rgba >> 24) & 0xFF),
			static_cast<uint8_t>((rgba >> 16) & 0xFF),
			static_cast<uint8_t>((rgba >> 8) & 0xFF),
			static_cast<uint8_t>((rgba) & 0xFF),
		};
	}

	[[nodiscard]] static constexpr auto from_rgb(uint8_t r, uint8_t g, uint8_t b) noexcept -> color {
		return {r, g, b, 255};
	}

	[[nodiscard]] static constexpr auto from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept -> color {
		return {r, g, b, a};
	}

	[[nodiscard]] constexpr auto operator==(const color &other) const noexcept -> bool = default;
};

// =============================================================================
// Common Colors
// =============================================================================

namespace colors {

// Neutrals
inline constexpr color white = color::from_hex(0xFFFFFFFF);
inline constexpr color black = color::from_hex(0x000000FF);
inline constexpr color transparent = color::from_hex(0x00000000);

// Grays
inline constexpr color gray_10 = color::from_hex(0x1A1A1AFF);
inline constexpr color gray_20 = color::from_hex(0x333333FF);
inline constexpr color gray_30 = color::from_hex(0x4D4D4DFF);
inline constexpr color gray_40 = color::from_hex(0x666666FF);
inline constexpr color gray_50 = color::from_hex(0x808080FF);
inline constexpr color gray_60 = color::from_hex(0x999999FF);
inline constexpr color gray_70 = color::from_hex(0xB3B3B3FF);
inline constexpr color gray_80 = color::from_hex(0xCCCCCCFF);
inline constexpr color gray_90 = color::from_hex(0xE6E6E6FF);

// Reds
inline constexpr color red = color::from_hex(0xFF0000FF);
inline constexpr color dark_red = color::from_hex(0x8B0000FF);
inline constexpr color light_red = color::from_hex(0xFF6666FF);

// Greens
inline constexpr color green = color::from_hex(0x00FF00FF);
inline constexpr color dark_green = color::from_hex(0x006400FF);
inline constexpr color light_green = color::from_hex(0x90EE90FF);
inline constexpr color lime = color::from_hex(0x32CD32FF);

// Blues
inline constexpr color blue = color::from_hex(0x0000FFFF);
inline constexpr color dark_blue = color::from_hex(0x00008BFF);
inline constexpr color light_blue = color::from_hex(0xADD8E6FF);
inline constexpr color sky_blue = color::from_hex(0x87CEEBFF);

// Yellows
inline constexpr color yellow = color::from_hex(0xFFFF00FF);
inline constexpr color dark_yellow = color::from_hex(0x9B870CFF);
inline constexpr color light_yellow = color::from_hex(0xFFFFE0FF);
inline constexpr color gold = color::from_hex(0xFFD700FF);

// Oranges
inline constexpr color orange = color::from_hex(0xFFA500FF);
inline constexpr color dark_orange = color::from_hex(0xFF8C00FF);

// Purples
inline constexpr color purple = color::from_hex(0x800080FF);
inline constexpr color violet = color::from_hex(0xEE82EEFF);
inline constexpr color dark_purple = color::from_hex(0x4B0082FF);

// Pinks
inline constexpr color pink = color::from_hex(0xFFC0CBFF);
inline constexpr color hot_pink = color::from_hex(0xFF69B4FF);
inline constexpr color deep_pink = color::from_hex(0xFF1493FF);

// Browns
inline constexpr color brown = color::from_hex(0xA52A2AFF);
inline constexpr color dark_brown = color::from_hex(0x5C3317FF);
inline constexpr color light_brown = color::from_hex(0xC4A35AFF);
inline constexpr color beige = color::from_hex(0xF5F5DCFF);

// Cyans
inline constexpr color cyan = color::from_hex(0x00FFFFFF);
inline constexpr color dark_cyan = color::from_hex(0x008B8BFF);
inline constexpr color teal = color::from_hex(0x008080FF);

// Magentas
inline constexpr color magenta = color::from_hex(0xFF00FFFF);

} // namespace colors

} // namespace lge