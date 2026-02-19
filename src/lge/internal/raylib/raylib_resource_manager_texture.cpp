// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include "raylib_resource_manager.hpp"

#include <raylib.h>

#include <string>
#include <string_view>

namespace lge {

// =============================================================================
// Texture
// =============================================================================

auto raylib_resource_manager::load_texture(const std::string_view uri) -> result<texture_handle> {
	log::debug("loading texture from uri `{}`", uri);

	if(!exists(uri)) [[unlikely]] {
		return error("texture file does not exist: " + std::string(uri));
	}

	const auto key = uri_to_key(uri);
	auto [it, inserted] = texture_cache_.load(key, uri);

	if(it == texture_cache_.end() || !it->second) [[unlikely]] {
		return error("failed to load texture from uri: " + std::string(uri));
	}

	return texture_handle::from_id(key);
}

auto raylib_resource_manager::unload_texture(const texture_handle handle) -> result<> {
	if(!handle.is_valid()) [[unlikely]] {
		return error("invalid texture handle");
	}

	const auto resource = texture_cache_[handle.raw()];
	if(!resource) [[unlikely]] {
		return error("texture not found in cache");
	}

	log::debug("unloading texture with id `{}` name `{}`", handle, resource->uri);
	texture_cache_.erase(handle.raw());

	return true;
}

auto raylib_resource_manager::get_raylib_texture(const texture_handle handle) const -> result<Texture2D> {
	if(!handle.is_valid()) [[unlikely]] {
		return error("invalid texture handle");
	}

	const auto resource = texture_cache_[handle.raw()];
	if(!resource) [[unlikely]] {
		return error("texture not found in cache");
	}

	return resource->raylib_texture;
}

} // namespace lge