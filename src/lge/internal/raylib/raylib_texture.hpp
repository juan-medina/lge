// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/internal/resource_manager/resource_store.hpp>

#include <raylib.h>

namespace lge {

	class raylib_texture {
	public:
		raylib_texture() = default;
		~raylib_texture();
		raylib_texture(const raylib_texture &) = delete;
		auto operator=(const raylib_texture &) -> raylib_texture & = delete;
		raylib_texture(raylib_texture &&) noexcept = default;
		auto operator=(raylib_texture &&) noexcept -> raylib_texture & = default;

		[[nodiscard]] auto load(std::string_view uri) -> result<>;

		Texture2D raylib_native_texture{};
	};

	using texture_store = resource_store<raylib_texture, texture_handle>;

} // namespace lge