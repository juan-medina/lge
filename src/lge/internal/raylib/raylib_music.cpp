// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/internal/raylib/raylib_music.hpp>

#include <raylib.h>

#include <string>
#include <string_view>

namespace lge {

raylib_music::~raylib_music() {
	if(raylib_native_music.stream.buffer != nullptr) {
		if(IsMusicStreamPlaying(raylib_native_music)) {
			StopMusicStream(raylib_native_music);
		}
		UnloadMusicStream(raylib_native_music);
	}
}

auto raylib_music::load(const std::string_view uri) -> result<> {
	raylib_native_music = LoadMusicStream(std::string(uri).c_str());
	if(!IsMusicValid(raylib_native_music)) {
		return error("invalid music loaded from uri: " + std::string(uri));
	}
	log::debug("loaded music from uri `{}` ", uri);
	return true;
}

} // namespace lge