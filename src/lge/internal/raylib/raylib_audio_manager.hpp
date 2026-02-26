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

	[[nodiscard]] auto play(sound_handle handle) noexcept -> result<> override;
	auto stop_all() noexcept -> void override;

private:
	raylib_resource_manager &resource_manager_; // NOLINT(*-avoid-const-or-ref-data-members)
};

} // namespace lge