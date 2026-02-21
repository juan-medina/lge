// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/internal/raylib/raylib_texture.hpp>

#include <raylib.h>

#include <string>
#include <string_view>

namespace lge {

raylib_texture::~raylib_texture() {
	if(raylib_native_texture.id != 0) {
		UnloadTexture(raylib_native_texture);
	}
}

auto raylib_texture::load(const std::string_view uri) -> result<> {
	raylib_native_texture = LoadTexture(std::string(uri).c_str());
	if(raylib_native_texture.id == 0) [[unlikely]] {
		return error("failed to load texture from uri: " + std::string(uri));
	}
	SetTextureFilter(raylib_native_texture, TEXTURE_FILTER_POINT);
	log::debug("loaded texture from uri `{}` with texture id {}", uri, raylib_native_texture.id);
	return true;
}

} // namespace lge