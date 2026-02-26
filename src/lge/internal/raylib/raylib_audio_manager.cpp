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

auto raylib_audio_manager::play(const sound_handle handle) noexcept -> result<> {
	Sound native{};
	if(const auto err = resource_manager_.get_raylib_sound(handle).unwrap(native); err) [[unlikely]] {
		return error("sound handle not found", *err);
	}
	PlaySound(native);
	return true;
}

auto raylib_audio_manager::stop_all() noexcept -> void {
	resource_manager_.for_each_sound([](const Sound &sound) -> void {
		if(IsSoundPlaying(sound)) {
			StopSound(sound);
		}
	});
}

} // namespace lge