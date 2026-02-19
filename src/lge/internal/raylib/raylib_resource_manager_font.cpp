// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include "raylib_resource_manager.hpp"

#include <raylib.h>

#include <entt/entt.hpp>
#include <string>
#include <string_view>

namespace lge {

// =============================================================================
// Font
// =============================================================================

auto raylib_resource_manager::load_font(const std::string_view uri) -> result<font_handle> {
	log::debug("loading font from uri `{}`", uri);

	if(!exists(uri)) [[unlikely]] {
		return error("font file does not exist: " + std::string(uri));
	}

	const auto key = uri_to_key(uri);
	auto [it, inserted] = font_cache_.load(key, uri);

	if(it == font_cache_.end() || !it->second) [[unlikely]] {
		return error("failed to load font from uri: " + std::string(uri));
	}

	return font_handle::from_id(key);
}

auto raylib_resource_manager::unload_font(const font_handle handle) -> result<> {
	if(!handle.is_valid()) [[unlikely]] {
		return error("invalid font handle");
	}

	const auto resource = font_cache_[handle.raw()];
	if(!resource) [[unlikely]] {
		return error("font not found in cache");
	}

	log::debug("unloading font with id `{}` name `{}`", handle, resource->uri);
	font_cache_.erase(handle.raw());

	return true;
}

auto raylib_resource_manager::get_raylib_font(const font_handle handle) const -> result<Font> {
	if(!handle.is_valid()) [[unlikely]] {
		return error("invalid font handle");
	}

	const auto resource = font_cache_[handle.raw()];
	if(!resource) [[unlikely]] {
		return error("font not found in cache");
	}

	return resource->raylib_font;
}

} // namespace lge