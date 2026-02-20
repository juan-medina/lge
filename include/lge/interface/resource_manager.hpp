// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/interface/resource_store.hpp>

#include <core/fwd.hpp>
#include <entt/entt.hpp>
#include <format>
#include <glm/ext/vector_float2.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
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
	std::vector<std::string> frames;
	float fps;
};

struct animation_library_data {
	std::string uri;
	sprite_sheet_handle sprite_sheet;
	std::unordered_map<std::string, animation_library_anim> animations;
};

// =============================================================================
// Resource Manager
// =============================================================================

class resource_manager {
public:
	resource_manager() = default;
	virtual ~resource_manager() = default;

	// Disable copying and moving — resource_manager is not copyable or movable
	resource_manager(const resource_manager &) = delete;
	resource_manager(resource_manager &&) = delete;
	auto operator=(const resource_manager &) -> resource_manager & = delete;
	auto operator=(resource_manager &&) -> resource_manager & = delete;

	[[nodiscard]] virtual auto init() -> result<> = 0;
	[[nodiscard]] virtual auto end() -> result<> = 0;

	// =============================================================================
	// Common
	// =============================================================================
	[[nodiscard]] virtual auto exists(std::string_view uri) const -> bool;

	// =============================================================================
	// Font
	// =============================================================================

	[[nodiscard]] virtual auto load_font(std::string_view uri) -> result<font_handle> = 0;
	[[nodiscard]] virtual auto unload_font(font_handle handle) -> result<> = 0;

	// =============================================================================
	// Texture
	// =============================================================================

	[[nodiscard]] virtual auto load_texture(std::string_view uri) -> result<texture_handle> = 0;
	[[nodiscard]] virtual auto unload_texture(texture_handle handle) -> result<> = 0;

	// =============================================================================
	// Sprite Sheet
	// =============================================================================

	[[nodiscard]] auto load_sprite_sheet(std::string_view uri) -> result<sprite_sheet_handle>;
	[[nodiscard]] auto unload_sprite_sheet(sprite_sheet_handle handle) -> result<>;
	[[nodiscard]] auto get_sprite_sheet_frame(sprite_sheet_handle handle, std::string_view frame_name) const
		-> result<sprite_sheet_frame>;
	[[nodiscard]] auto get_sprite_sheet_texture(sprite_sheet_handle handle) const -> result<texture_handle>;

	// =============================================================================
	// Animation Library
	// =============================================================================
	[[nodiscard]] virtual auto load_animation_library(std::string_view uri) -> result<animation_library_handle>;
	[[nodiscard]] virtual auto unload_animation_library(animation_library_handle handle) -> result<>;
	[[nodiscard]] virtual auto get_animation(animation_library_handle handle, std::string_view anim_name) const
		-> result<animation_library_anim>;
	[[nodiscard]] virtual auto get_animation_sprite_sheet(animation_library_handle handle) const
		-> result<sprite_sheet_handle>;

private:
	// =============================================================================
	// Sprite Sheet Data
	// =============================================================================

	class sprite_sheet_data {
	public:
		[[nodiscard]] auto load(std::string_view uri, resource_manager &rm) -> result<>;
		texture_handle texture;
		std::unordered_map<std::string, sprite_sheet_frame> frames;
	};

	resource_store<sprite_sheet_data, sprite_sheet_handle> sprite_sheets_;

	// =============================================================================
	// Animation Library Data
	// =============================================================================

	std::unordered_map<entt::id_type, animation_library_data> animation_libraries_;

	static auto uri_to_key(const std::string_view uri) noexcept -> entt::id_type {
		return entt::hashed_string{uri.data()}.value(); // NOLINT(*-suspicious-stringview-data-usage)
	}
};

} //  namespace lge

template<typename Tag>
struct std::formatter<lge::resource_handle<Tag>> {
	constexpr auto parse(std::format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(const lge::resource_handle<Tag> &handle, std::format_context &ctx) const {
		return std::format_to(ctx.out(), "{}", handle.raw());
	}
};