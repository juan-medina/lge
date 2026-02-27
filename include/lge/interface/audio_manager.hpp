// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/interface/resources.hpp>

namespace lge {

// =============================================================================
// Audio Manager
// =============================================================================

class audio_manager {
public:
	audio_manager() = default;
	virtual ~audio_manager() = default;

	// Disable copying and moving — audio_manager is not copyable or movable
	audio_manager(const audio_manager &) = delete;
	audio_manager(audio_manager &&) = delete;
	auto operator=(const audio_manager &) -> audio_manager & = delete;
	auto operator=(audio_manager &&) -> audio_manager & = delete;

	[[nodiscard]] virtual auto init() -> result<> = 0;
	[[nodiscard]] virtual auto end() -> result<> = 0;

	[[nodiscard]] virtual auto play_sound(sound_handle handle) noexcept -> result<> = 0;

	[[nodiscard]] virtual auto play_music(music_handle handle) noexcept -> result<> = 0;
	[[nodiscard]] virtual auto stop_music() noexcept -> result<> = 0;
	[[nodiscard]] virtual auto pause_music() noexcept -> result<> = 0;
	[[nodiscard]] virtual auto resume_music() noexcept -> result<> = 0;
	[[nodiscard]] virtual auto is_music_playing() const noexcept -> bool = 0;
	[[nodiscard]] virtual auto update_music() noexcept -> result<> = 0;

	virtual auto stop_all() noexcept -> void = 0;
};

} // namespace lge