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

raylib_resource_manager::texture_data::~texture_data() {
	if(raylib_texture.id != 0) {
		UnloadTexture(raylib_texture);
	}
}

auto raylib_resource_manager::texture_data::load(const std::string_view uri) -> result<> {
	raylib_texture = LoadTexture(std::string(uri).c_str());
	if(raylib_texture.id == 0) [[unlikely]] {
		return error("failed to load texture from uri: " + std::string(uri));
	}
	SetTextureFilter(raylib_texture, TEXTURE_FILTER_POINT);
	return true;
}

auto raylib_resource_manager::load_texture(const std::string_view uri) -> result<texture_handle> {
	log::debug("loading texture from uri `{}`", uri);

	if(!exists(uri)) [[unlikely]] {
		return error("texture file does not exist: " + std::string(uri));
	}

	texture_handle handle;
	if(const auto err = textures_.load(uri).unwrap(handle); err) [[unlikely]] {
		return error("failed to load texture", *err);
	}
	return handle;
}

auto raylib_resource_manager::unload_texture(const texture_handle handle) -> result<> {
	if(const auto err = textures_.unload(handle).unwrap(); err) [[unlikely]] {
		return error("failed to unload texture", *err);
	}
	return true;
}

auto raylib_resource_manager::get_raylib_texture(const texture_handle handle) const -> result<Texture2D> {
	const texture_data *data = nullptr;
	if(const auto err = textures_.get(handle).unwrap(data); err) [[unlikely]] {
		return error("texture not found", *err);
	}
	return data->raylib_texture;
}

} // namespace lge