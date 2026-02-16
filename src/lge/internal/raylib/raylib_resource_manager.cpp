// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "raylib_resource_manager.hpp"

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include <raylib.h>

#include <string>

namespace lge {

auto raylib_resource_manager::init() -> result<> {
	log::debug("initializing raylib resource manager");
	return true;
}

auto raylib_resource_manager::end() -> result<> {
	log::debug("ending raylib resource manager");
	return true;
}

auto raylib_resource_manager::load_font(const resource_uri &uri) -> result<font_handle> {
	log::debug("loading font from uri `{}`", uri);

	if(!uri.exists()) [[unlikely]] {
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

	log::debug("unloading font with id `{}`", handle);
	font_cache_.erase(handle.raw());

	return true;
}

auto raylib_resource_manager::is_font_loaded(const font_handle handle) const noexcept -> bool {
	return handle.is_valid() && font_cache_[handle.raw()];
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

auto raylib_resource_manager::load_texture(const resource_uri &uri) -> result<texture_handle> {
	log::debug("loading texture from uri `{}`", uri);

	if(!uri.exists()) [[unlikely]] {
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

	log::debug("unloading texture with id `{}`", handle);
	texture_cache_.erase(handle.raw());

	return true;
}

auto raylib_resource_manager::is_texture_loaded(const texture_handle handle) const noexcept -> bool {
	return handle.is_valid() && texture_cache_[handle.raw()];
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