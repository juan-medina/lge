// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>
#include <lge/interface/resources.hpp>
#include <lge/internal/resource_manager/resource_store.hpp>

#include <core/fwd.hpp>
#include <entt/entt.hpp>
#include <string_view>
#include <unordered_map>

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
	// =============================================================================
	// Sprite Sheet Data
	// =============================================================================

	class sprite_sheet_data {
	public:
		~sprite_sheet_data();
		sprite_sheet_data() = default;
		sprite_sheet_data(const sprite_sheet_data &) = delete;
		auto operator=(const sprite_sheet_data &) -> sprite_sheet_data & = delete;
		sprite_sheet_data(sprite_sheet_data &&) noexcept = default;
		auto operator=(sprite_sheet_data &&) noexcept -> sprite_sheet_data & = default;

		[[nodiscard]] auto load(std::string_view uri, resource_manager &rm) -> result<>;
		texture_handle texture;
		std::unordered_map<entt::id_type, sprite_sheet_frame> frames;

	private:
		resource_manager *rm_ = nullptr;
	};

	resource_store<sprite_sheet_data, sprite_sheet_handle> sprite_sheets_;

	// =============================================================================
	// Animation Library Data
	// =============================================================================

	class animation_library_data {
	public:
		~animation_library_data();
		animation_library_data() = default;
		animation_library_data(const animation_library_data &) = delete;
		auto operator=(const animation_library_data &) -> animation_library_data & = delete;
		animation_library_data(animation_library_data &&) noexcept = default;
		auto operator=(animation_library_data &&) noexcept -> animation_library_data & = default;

		[[nodiscard]] auto load(std::string_view uri, resource_manager &rm) -> result<>;
		sprite_sheet_handle sprite_sheet;
		std::unordered_map<entt::id_type, animation_library_anim> animations;

	private:
		resource_manager *rm_ = nullptr;
	};

	resource_store<animation_library_data, animation_library_handle> animation_libraries_;
};

} //  namespace lge
