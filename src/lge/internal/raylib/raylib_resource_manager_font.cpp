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
// Font
// =============================================================================

raylib_resource_manager::font_data::~font_data() {
	if(raylib_font.texture.id != 0) {
		UnloadFont(raylib_font);
	}
}

auto raylib_resource_manager::font_data::load(const std::string_view uri) -> result<> {
	static const auto raylib_default_font_texture_id = GetFontDefault().texture.id;
	raylib_font = LoadFont(std::string(uri).c_str());
	// raylib returns the default font if it fails to load a font, so we check if the texture id is
	// the same as the default font's texture id to determine if loading failed.
	if(raylib_font.texture.id == raylib_default_font_texture_id) [[unlikely]] {
		return error("failed to load font from uri: " + std::string(uri));
	}
	SetTextureFilter(raylib_font.texture, TEXTURE_FILTER_POINT);
	return true;
}

auto raylib_resource_manager::load_font(const std::string_view uri) -> result<font_handle> {
	log::debug("loading font from uri `{}`", uri);

	if(!exists(uri)) [[unlikely]] {
		return error("font file does not exist: " + std::string(uri));
	}

	font_handle handle;
	if(const auto err = fonts_.load(uri).unwrap(handle); err) [[unlikely]] {
		return error("failed to load font", *err);
	}
	return handle;
}

auto raylib_resource_manager::unload_font(const font_handle handle) -> result<> {
	if(const auto err = fonts_.unload(handle).unwrap(); err) [[unlikely]] {
		return error("failed to unload font", *err);
	}
	return true;
}

auto raylib_resource_manager::get_raylib_font(const font_handle handle) const -> result<Font> {
	const font_data *data = nullptr;
	if(const auto err = fonts_.get(handle).unwrap(data); err) [[unlikely]] {
		return error("font not found", *err);
	}
	return data->raylib_font;
}

} // namespace lge