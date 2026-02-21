// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/interface/resources.hpp>
#include <lge/internal/raylib/raylib_font.hpp>
#include <lge/internal/raylib/raylib_texture.hpp>
#include <lge/internal/resource_manager/base_resource_manager.hpp>

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
	texture_store textures_;
};

} // namespace lge