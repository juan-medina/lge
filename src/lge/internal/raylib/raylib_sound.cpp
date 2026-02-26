// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/internal/raylib/raylib_sound.hpp>

#include <raylib.h>

#include <string>
#include <string_view>

namespace lge {

raylib_sound::~raylib_sound() {
	if(raylib_native_sound.stream.buffer != nullptr) {
		if(IsSoundPlaying(raylib_native_sound)) {
			StopSound(raylib_native_sound);
		}
		UnloadSound(raylib_native_sound);
	}
}

auto raylib_sound::load(const std::string_view uri) -> result<> {
	raylib_native_sound = LoadSound(std::string(uri).c_str());
	if(raylib_native_sound.stream.buffer == nullptr) [[unlikely]] {
		return error("failed to load sound from uri: " + std::string(uri));
	}
	log::debug("loaded sound from uri `{}` ", uri);
	return true;
}

} // namespace lge