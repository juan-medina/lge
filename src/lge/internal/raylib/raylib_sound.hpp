// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/interface/resources.hpp>
#include <lge/internal/resource_manager/resource_store.hpp>

#include <raylib.h>

#include <string_view>

namespace lge {

class raylib_sound {
public:
	raylib_sound() = default;
	~raylib_sound();
	raylib_sound(const raylib_sound &) = delete;
	auto operator=(const raylib_sound &) -> raylib_sound & = delete;
	raylib_sound(raylib_sound &&) noexcept = default;
	auto operator=(raylib_sound &&) noexcept -> raylib_sound & = default;

	[[nodiscard]] auto load(std::string_view uri) -> result<>;

	Sound raylib_native_sound{};
};

using sound_store = resource_store<raylib_sound, sound_handle>;

} // namespace lge