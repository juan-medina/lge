// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/interface/resources.hpp>
#include <lge/internal/resource_manager/resource_store.hpp>

#include <raylib.h>

#include <string_view>

namespace lge {

class raylib_font {
public:
	raylib_font() = default;
	~raylib_font();
	raylib_font(const raylib_font &) = delete;
	auto operator=(const raylib_font &) -> raylib_font & = delete;
	raylib_font(raylib_font &&) noexcept = default;
	auto operator=(raylib_font &&) noexcept -> raylib_font & = default;

	[[nodiscard]] auto load(std::string_view uri) -> result<>;

	Font raylib_native_font{};
};

using font_store = resource_store<raylib_font, font_handle>;

} // namespace lge