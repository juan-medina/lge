// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/interface/resources.hpp>

#include <core/fwd.hpp>
#include <entt/entt.hpp>
#include <string_view>

namespace lge {

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

	[[nodiscard]] virtual auto load_sprite_sheet(std::string_view uri) -> result<sprite_sheet_handle> = 0;
	[[nodiscard]] virtual auto unload_sprite_sheet(sprite_sheet_handle handle) -> result<> = 0;
	[[nodiscard]] virtual auto get_sprite_sheet_frame(sprite_sheet_handle handle, entt::id_type frame_name) const
		-> result<sprite_sheet_frame> = 0;
	[[nodiscard]] virtual auto get_sprite_sheet_texture(sprite_sheet_handle handle) const -> result<texture_handle> = 0;

	// =============================================================================
	// Animation Library
	// =============================================================================
	[[nodiscard]] virtual auto load_animation_library(std::string_view uri) -> result<animation_library_handle> = 0;
	[[nodiscard]] virtual auto unload_animation_library(animation_library_handle handle) -> result<> = 0;
	[[nodiscard]] virtual auto get_animation(animation_library_handle handle, entt::id_type anim_name) const
		-> result<animation_library_anim> = 0;
	[[nodiscard]] virtual auto get_animation_sprite_sheet(animation_library_handle handle) const
		-> result<sprite_sheet_handle> = 0;
};

} //  namespace lge
