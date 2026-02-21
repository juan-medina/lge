// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <entt/core/fwd.hpp>
#include <entt/entity/entity.hpp>
#include <format>
#include <glm/ext/vector_float2.hpp>
#include <vector>

namespace lge {

// =============================================================================
// Type-Safe Resource Handles
// =============================================================================

template<typename Tag>
struct resource_handle {
	entt::id_type id = entt::null;

	[[nodiscard]] constexpr auto is_valid() const noexcept -> bool {
		return id != entt::null;
	}

	constexpr auto operator==(const resource_handle &other) const noexcept -> bool = default;

	[[nodiscard]] constexpr auto raw() const noexcept -> entt::id_type {
		return id;
	}

	static constexpr auto from_id(entt::id_type id) noexcept -> resource_handle {
		return resource_handle{id};
	}
};

struct font_tag {};
using font_handle = resource_handle<font_tag>;
inline constexpr font_handle invalid_font{};

struct texture_tag {};
using texture_handle = resource_handle<texture_tag>;
inline constexpr texture_handle invalid_texture{};

struct sprite_sheet_tag {};
using sprite_sheet_handle = resource_handle<sprite_sheet_tag>;
inline constexpr sprite_sheet_handle invalid_sprite_sheet{};

struct animation_library_tag {};
using animation_library_handle = resource_handle<animation_library_tag>;
inline constexpr animation_library_handle invalid_animation_library{};

// =============================================================================
// Data
// =============================================================================

struct sprite_sheet_frame {
	glm::vec2 source_pos;
	glm::vec2 source_size;
	glm::vec2 pivot;
};

struct animation_library_anim {
	std::vector<entt::hashed_string> frames;
	float fps;
};

} // namespace lge

template<typename Tag>
struct std::formatter<lge::resource_handle<Tag>> {
	constexpr auto parse(std::format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(const lge::resource_handle<Tag> &handle, std::format_context &ctx) const {
		return std::format_to(ctx.out(), "{}", handle.raw());
	}
};