// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/internal/raylib/raylib_font.hpp>

#include <raylib.h>

#include <string>
#include <string_view>

namespace lge {

raylib_font::~raylib_font() {
	if(raylib_native_font.texture.id != 0) {
		UnloadFont(raylib_native_font);
	}
}

auto raylib_font::load(const std::string_view uri) -> result<> {
	static const auto raylib_default_raylib_font_texture_id = GetFontDefault().texture.id;
	raylib_native_font = LoadFont(std::string(uri).c_str());
	// raylib returns the default raylib_font if it fails to load a raylib_font, so we check if the texture id is
	// the same as the default raylib_font's texture id to determine if loading failed.
	if(raylib_native_font.texture.id == raylib_default_raylib_font_texture_id) [[unlikely]] {
		return error("failed to load raylib_font from uri: " + std::string(uri));
	}
	SetTextureFilter(raylib_native_font.texture, TEXTURE_FILTER_POINT);
	log::debug("loaded raylib_font from uri `{}` with texture id {}", uri, raylib_native_font.texture.id);
	return true;
}

} // namespace lge