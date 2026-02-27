// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/interface/audio_manager.hpp>
#include <lge/interface/resources.hpp>

namespace lge {

class raylib_resource_manager;

class raylib_audio_manager final: public audio_manager {
public:
	explicit raylib_audio_manager(raylib_resource_manager &resource_manager) noexcept
		: resource_manager_{resource_manager} {}

	[[nodiscard]] auto init() -> result<> override;
	[[nodiscard]] auto end() -> result<> override;

	[[nodiscard]] auto play_sound(sound_handle handle) noexcept -> result<> override;

	[[nodiscard]] auto play_music(music_handle handle) noexcept -> result<> override;
	[[nodiscard]] auto stop_music() noexcept -> result<> override;
	[[nodiscard]] auto pause_music() noexcept -> result<> override;
	[[nodiscard]] auto resume_music() noexcept -> result<> override;
	[[nodiscard]] auto is_music_playing() const noexcept -> bool override;
	[[nodiscard]] auto update_music() noexcept -> result<> override;

	auto stop_all() noexcept -> void override;

private:
	raylib_resource_manager &resource_manager_; // NOLINT(*-avoid-const-or-ref-data-members)
	music_handle current_music_{invalid_music};
};

} // namespace lge