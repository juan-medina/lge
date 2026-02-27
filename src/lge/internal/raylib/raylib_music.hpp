// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/interface/resources.hpp>
#include <lge/internal/resource_manager/resource_store.hpp>

#include <raylib.h>

#include <string_view>

namespace lge {

class raylib_music {
public:
	raylib_music() = default;
	~raylib_music();
	raylib_music(const raylib_music &) = delete;
	auto operator=(const raylib_music &) -> raylib_music & = delete;
	raylib_music(raylib_music &&) noexcept = default;
	auto operator=(raylib_music &&) noexcept -> raylib_music & = default;

	[[nodiscard]] auto load(std::string_view uri) -> result<>;

	Music raylib_native_music{};
};

using music_store = resource_store<raylib_music, music_handle>;

} // namespace lge