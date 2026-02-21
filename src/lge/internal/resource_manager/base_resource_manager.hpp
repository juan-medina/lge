// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>
#include <lge/interface/resources.hpp>
#include <lge/internal/resource_manager/animation_library.hpp>
#include <lge/internal/resource_manager/sprite_sheet.hpp>

#include <core/fwd.hpp>
#include <entt/entt.hpp>
#include <string_view>

namespace lge {

// =============================================================================
// Resource Manager
// =============================================================================

class base_resource_manager: public resource_manager {
public:
	base_resource_manager() = default;
	~base_resource_manager() override = default;

	// Disable copying and moving — base_resource_manager is not copyable or movable
	base_resource_manager(const base_resource_manager &) = delete;
	base_resource_manager(base_resource_manager &&) = delete;
	auto operator=(const base_resource_manager &) -> base_resource_manager & = delete;
	auto operator=(base_resource_manager &&) -> base_resource_manager & = delete;

	// =============================================================================
	// Common
	// =============================================================================
	[[nodiscard]] virtual auto exists(std::string_view uri) const -> bool;

	// =============================================================================
	// Sprite Sheet
	// =============================================================================

	[[nodiscard]] auto load_sprite_sheet(std::string_view uri) -> result<sprite_sheet_handle> override;
	[[nodiscard]] auto unload_sprite_sheet(sprite_sheet_handle handle) -> result<> override;
	[[nodiscard]] auto get_sprite_sheet_frame(sprite_sheet_handle handle, entt::id_type frame_name) const
		-> result<sprite_sheet_frame> override;
	[[nodiscard]] auto get_sprite_sheet_texture(sprite_sheet_handle handle) const -> result<texture_handle> override;

	// =============================================================================
	// Animation Library
	// =============================================================================
	[[nodiscard]] auto load_animation_library(std::string_view uri) -> result<animation_library_handle> override;
	[[nodiscard]] auto unload_animation_library(animation_library_handle handle) -> result<> override;
	[[nodiscard]] auto get_animation(animation_library_handle handle, entt::id_type anim_name) const
		-> result<animation_library_anim> override;
	[[nodiscard]] auto get_animation_sprite_sheet(animation_library_handle handle) const
		-> result<sprite_sheet_handle> override;

private:
	sprite_sheet_store sprite_sheets_;
	animation_library_store animation_libraries_;
};

} //  namespace lge
