// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "raylib_resource_manager.hpp"

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resources.hpp>
#include <lge/internal/raylib/raylib_font.hpp>
#include <lge/internal/raylib/raylib_music.hpp>
#include <lge/internal/raylib/raylib_sound.hpp>
#include <lge/internal/raylib/raylib_texture.hpp>

#include <raylib.h>

#include <functional>
#include <string>
#include <string_view>

namespace lge {

// =============================================================================
// Init / end
// =============================================================================

auto raylib_resource_manager::init() -> result<> {
	log::debug("initializing raylib resource manager");
	return true;
}

auto raylib_resource_manager::end() -> result<> {
	log::debug("ending raylib resource manager");
	return true;
}

// =============================================================================
// Font
// =============================================================================

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
	const raylib_font *data = nullptr;
	if(const auto err = fonts_.get(handle).unwrap(data); err) [[unlikely]] {
		return error("font not found", *err);
	}
	return data->raylib_native_font;
}

// =============================================================================
// Texture
// =============================================================================

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
	const raylib_texture *data = nullptr;
	if(const auto err = textures_.get(handle).unwrap(data); err) [[unlikely]] {
		return error("texture not found", *err);
	}
	return data->raylib_native_texture;
}

// =============================================================================
// Sound
// =============================================================================

auto raylib_resource_manager::load_sound(const std::string_view uri) -> result<sound_handle> {
	log::debug("loading sound from uri `{}`", uri);

	if(!exists(uri)) [[unlikely]] {
		return error("sound file does not exist: " + std::string(uri));
	}

	sound_handle handle;
	if(const auto err = sounds_.load(uri).unwrap(handle); err) [[unlikely]] {
		return error("failed to load sound", *err);
	}
	return handle;
}

auto raylib_resource_manager::unload_sound(const sound_handle handle) -> result<> {
	if(const auto err = sounds_.unload(handle).unwrap(); err) [[unlikely]] {
		return error("failed to unload sound", *err);
	}
	return true;
}

auto raylib_resource_manager::get_raylib_sound(const sound_handle handle) const -> result<Sound> {
	const raylib_sound *data = nullptr;
	if(const auto err = sounds_.get(handle).unwrap(data); err) [[unlikely]] {
		return error("sound not found", *err);
	}
	return data->raylib_native_sound;
}

auto raylib_resource_manager::for_each_sound(std::function<void(Sound)> fn) const -> void {
	sounds_.for_each([&fn](const raylib_sound &s) -> void { fn(s.raylib_native_sound); });
}

auto raylib_resource_manager::load_music(std::string_view uri) -> result<music_handle> {
	log::debug("loading music from uri `{}`", uri);

	if(!exists(uri)) [[unlikely]] {
		return error("music file does not exist: " + std::string(uri));
	}

	music_handle handle;
	if(const auto err = musics_.load(uri).unwrap(handle); err) [[unlikely]] {
		return error("failed to load music", *err);
	}
	return handle;
}

auto raylib_resource_manager::unload_music(const music_handle handle) -> result<> {
	if(const auto err = musics_.unload(handle).unwrap(); err) [[unlikely]] {
		return error("failed to unload sound", *err);
	}
	return true;
}

auto raylib_resource_manager::get_raylib_music(music_handle handle) const -> result<Music> {
	const raylib_music *data = nullptr;
	if(const auto err = musics_.get(handle).unwrap(data); err) [[unlikely]] {
		return error("sound not found", *err);
	}
	return data->raylib_native_music;
}

} // namespace lge