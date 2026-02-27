// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resources.hpp>
#include <lge/internal/raylib/raylib_audio_manager.hpp>
#include <lge/internal/raylib/raylib_resource_manager.hpp>

#include <raylib.h>

namespace lge {

auto raylib_audio_manager::init() -> result<> {
	log::debug("initializing raylib audio manager");
	InitAudioDevice();
	if(!IsAudioDeviceReady()) [[unlikely]] {
		return error("failed to initialize audio device");
	}
	return true;
}

auto raylib_audio_manager::end() -> result<> {
	log::debug("ending raylib audio manager");
	stop_all();
	CloseAudioDevice();
	return true;
}

auto raylib_audio_manager::play_sound(const sound_handle handle) noexcept -> result<> {
	Sound native{};
	if(const auto err = resource_manager_.get_raylib_sound(handle).unwrap(native); err) [[unlikely]] {
		return error("sound handle not found", *err);
	}
	PlaySound(native);
	return true;
}

auto raylib_audio_manager::play_music(const music_handle handle) noexcept -> result<> {
	if(current_music_ != invalid_music) {
		if(const auto err = stop_music().unwrap(); err) [[unlikely]] {
			return error("failed to stop current music", *err);
		}
	}
	current_music_ = handle;

	Music native{};
	if(const auto err = resource_manager_.get_raylib_music(handle).unwrap(native); err) [[unlikely]] {
		return error("music handle not found", *err);
	}
	PlayMusicStream(native);

	return true;
}

auto raylib_audio_manager::stop_music() noexcept -> result<> {
	if(current_music_ == invalid_music) {
		log::warn("attempted to stop music when no music is currently playing");
		return true;
	}

	Music native{};
	if(const auto err = resource_manager_.get_raylib_music(current_music_).unwrap(native); err) [[unlikely]] {
		return error("current music handle not found", *err);
	}

	if(IsMusicStreamPlaying(native)) {
		StopMusicStream(native);
	}

	current_music_ = invalid_music;
	return true;
}

auto raylib_audio_manager::pause_music() noexcept -> result<> {
	if(current_music_ == invalid_music) {
		log::warn("attempted to pause music when no music is currently playing");
		return true;
	}

	Music native{};
	if(const auto err = resource_manager_.get_raylib_music(current_music_).unwrap(native); err) [[unlikely]] {
		return error("current music handle not found", *err);
	}

	if(IsMusicStreamPlaying(native)) {
		PauseMusicStream(native);
	}

	return true;
}

auto raylib_audio_manager::resume_music() noexcept -> result<> {
	if(current_music_ == invalid_music) {
		log::warn("attempted to resume music when no music is currently playing");
		return true;
	}

	Music native{};
	if(const auto err = resource_manager_.get_raylib_music(current_music_).unwrap(native); err) [[unlikely]] {
		return error("current music handle not found", *err);
	}

	ResumeMusicStream(native);

	return true;
}

auto raylib_audio_manager::is_music_playing() const noexcept -> bool {
	if(current_music_ == invalid_music) {
		return false;
	}

	Music native{};
	if(const auto err = resource_manager_.get_raylib_music(current_music_).unwrap(native); err) [[unlikely]] {
		log::warn("failed to get current music for is_music_playing check: {}", err->get_message());
		return false;
	}

	return IsMusicStreamPlaying(native);
}

auto raylib_audio_manager::update_music() noexcept -> result<> {
	if(current_music_ == invalid_music) {
		return true;
	}

	Music native{};
	if(const auto err = resource_manager_.get_raylib_music(current_music_).unwrap(native); err) [[unlikely]] {
		return error("current music handle not found", *err);
	}

	if(IsMusicStreamPlaying(native)) {
		UpdateMusicStream(native);
	}
	return true;
}

auto raylib_audio_manager::stop_all() noexcept -> void {
	resource_manager_.for_each_sound([](const Sound &sound) -> void {
		if(IsSoundPlaying(sound)) {
			StopSound(sound);
		}
	});
	if(is_music_playing()) {
		if(const auto err = stop_music().unwrap(); err) [[unlikely]] {
			log::warn("failed to stop music in stop_all: {}", err->get_message());
		}
	}
}

} // namespace lge