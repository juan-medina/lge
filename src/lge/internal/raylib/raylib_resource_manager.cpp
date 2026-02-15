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
	LGE_DEBUG("initializing raylib resource manager");
	return true;
}

auto raylib_resource_manager::end() -> result<> {
	LGE_DEBUG("ending raylib resource manager");
	return true;
}

auto raylib_resource_manager::load_font(const resource_uri &uri) -> result<font_id> {
	LGE_DEBUG("loading font from uri `{}`", uri);
	if(!uri.exists()) [[unlikely]] {
		return error("font file does not exist: " + std::string(uri));
	}

	const auto key = entt::hashed_string{static_cast<std::string>(uri).c_str()}.value();
	if(auto [it, inserted] = font_cache_.load(key, uri); it == font_cache_.end() || !it->second) {
		return error("failed to load font from uri: " + std::string(uri));
	}

	return key;
}

auto raylib_resource_manager::unload_font(const font_id id) -> result<> {
	const auto handle = font_cache_[id]; // NOLINT(*-pro-bounds-avoid-unchecked-container-access)
	if(!handle) {
		LGE_ERROR("failed to unload font with id `{}`: font not found in cache", id);
		return error("font not found in cache");
	}

	font_cache_.erase(id);

	LGE_DEBUG("unloading font from uri `{}`", handle->uri);

	return true;
}

auto raylib_resource_manager::get_raylib_font(font_id id) const -> result<Font> {
	const auto handle = font_cache_[id]; // NOLINT(*-pro-bounds-avoid-unchecked-container-access)
	if(!handle) {
		LGE_ERROR("failed to get raylib font with id `{}`: font not found in cache", id);
		return error("font not found in cache");
	}

	return handle->raylib_font;
}

} // namespace lge