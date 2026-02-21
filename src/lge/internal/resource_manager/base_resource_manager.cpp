// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/result.hpp>
#include <lge/interface/resources.hpp>
#include <lge/internal/resource_manager/animation_library.hpp>
#include <lge/internal/resource_manager/base_resource_manager.hpp>
#include <lge/internal/resource_manager/sprite_sheet.hpp>

#include <entt/core/fwd.hpp>
#include <filesystem>
#include <format>
#include <string_view>

namespace lge {

// =============================================================================
// Common
// =============================================================================

auto base_resource_manager::exists(const std::string_view uri) const -> bool {
	return std::filesystem::exists(uri);
}

auto base_resource_manager::load_sprite_sheet(const std::string_view uri) -> result<sprite_sheet_handle> {
	sprite_sheet_handle handle;
	if(const auto err = sprite_sheets_.load(uri, *this).unwrap(handle); err) {
		return error("failed to load sprite sheet", *err);
	}
	return handle;
}

auto base_resource_manager::unload_sprite_sheet(const sprite_sheet_handle handle) -> result<> {
	if(const auto err = sprite_sheets_.unload(handle).unwrap(); err) {
		return error("failed to unload sprite sheet", *err);
	}

	return true;
}

auto base_resource_manager::get_sprite_sheet_frame(const sprite_sheet_handle handle,
												   const entt::id_type frame_name) const -> result<sprite_sheet_frame> {
	const sprite_sheet *data = nullptr;
	if(const auto err = sprite_sheets_.get(handle).unwrap(data); err) [[unlikely]] {
		return error("can not get sprite sheet frame, sprite sheet not found", *err);
	}

	const auto it = data->frames.find(frame_name);
	if(it == data->frames.end()) [[unlikely]] {
		return error(std::format("sprite sheet frame not found: {}", frame_name));
	}
	return it->second;
}

auto base_resource_manager::get_sprite_sheet_texture(const sprite_sheet_handle handle) const -> result<texture_handle> {
	const sprite_sheet *data = nullptr;
	if(const auto err = sprite_sheets_.get(handle).unwrap(data); err) [[unlikely]] {
		return error("can not get sprite sheet texture, sprite sheet not found", *err);
	}
	return data->texture;
}

auto base_resource_manager::load_animation_library(const std::string_view uri) -> result<animation_library_handle> {
	animation_library_handle handle;
	if(const auto err = animation_libraries_.load(uri, *this).unwrap(handle); err) [[unlikely]] {
		return error("failed to load animation library", *err);
	}
	return handle;
}

auto base_resource_manager::unload_animation_library(const animation_library_handle handle) -> result<> {
	if(const auto err = animation_libraries_.unload(handle).unwrap(); err) [[unlikely]] {
		return error("failed to unload animation library", *err);
	}
	return true;
}

auto base_resource_manager::get_animation(const animation_library_handle handle, const entt::id_type anim_name) const
	-> result<animation_library_anim> {
	const animation_library *data = nullptr;
	if(const auto err = animation_libraries_.get(handle).unwrap(data); err) [[unlikely]] {
		return error("animation library not found", *err);
	}
	const auto clip_it = data->animations.find(anim_name);
	if(clip_it == data->animations.end()) [[unlikely]] {
		return error(std::format("animation clip '{}' not found in animation library", anim_name));
	}
	return clip_it->second;
}

auto base_resource_manager::get_animation_sprite_sheet(const animation_library_handle handle) const
	-> result<sprite_sheet_handle> {
	const animation_library *data = nullptr;
	if(const auto err = animation_libraries_.get(handle).unwrap(data); err) [[unlikely]] {
		return error("can not get animation sprite sheet, animation library not found", *err);
	}
	return data->sprite_sheet;
}

} // namespace lge