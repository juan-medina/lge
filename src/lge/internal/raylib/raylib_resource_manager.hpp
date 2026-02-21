// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/interface/resources.hpp>
#include <lge/internal/raylib/raylib_font.hpp>
#include <lge/internal/resource_manager/base_resource_manager.hpp>
#include <lge/internal/resource_manager/resource_store.hpp>

#include <raylib.h>

#include <string_view>

namespace lge {

class raylib_resource_manager final: public base_resource_manager {
public:
	[[nodiscard]] auto init() -> result<> override;
	[[nodiscard]] auto end() -> result<> override;

	[[nodiscard]] auto load_font(std::string_view uri) -> result<font_handle> override;
	[[nodiscard]] auto unload_font(font_handle handle) -> result<> override;
	[[nodiscard]] auto get_raylib_font(font_handle handle) const -> result<Font>;

	[[nodiscard]] auto load_texture(std::string_view uri) -> result<texture_handle> override;
	[[nodiscard]] auto unload_texture(texture_handle handle) -> result<> override;
	[[nodiscard]] auto get_raylib_texture(texture_handle handle) const -> result<Texture2D>;

private:
	font_store fonts_;

	// =============================================================================
	// Texture Data
	// =============================================================================

	class texture_data {
	public:
		texture_data() = default;
		~texture_data();
		texture_data(const texture_data &) = delete;
		auto operator=(const texture_data &) -> texture_data & = delete;
		texture_data(texture_data &&) noexcept = default;
		auto operator=(texture_data &&) noexcept -> texture_data & = default;
		[[nodiscard]] auto load(std::string_view uri) -> result<>;
		Texture2D raylib_texture{};
	};

	resource_store<texture_data, texture_handle> textures_;
};

} // namespace lge