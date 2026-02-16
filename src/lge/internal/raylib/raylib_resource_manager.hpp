// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include <raylib.h>

#include <entt/core/fwd.hpp>
#include <entt/entt.hpp>
#include <memory>
#include <string>

namespace lge {

struct font {
	Font raylib_font;
	resource_uri uri;
	explicit font(const Font &f, const resource_uri &uri): raylib_font(f), uri(uri) {}
	~font() {
		UnloadFont(raylib_font);
	}
	font(const font &) = delete;
	font(font &&) = delete;
	auto operator=(const font &) -> font & = delete;
	auto operator=(font &&) -> font & = delete;
};

struct font_loader: entt::resource_loader<font> {
	using result_type = std::shared_ptr<font>;

	auto operator()(const resource_uri &uri) const -> result_type {
		// we cache raylib's  default font's texture id
		static auto raylib_default_font_texture_id = GetFontDefault().texture.id;
		const auto rl = LoadFont(std::string(uri).c_str());
		// raylib returns the default font if it fails to load a font, so we check if the texture id is
		// the same as the default font's texture id to determine if loading failed.
		if(rl.texture.id == raylib_default_font_texture_id) [[unlikely]] {
			log::error("failed to load font from uri: {}", uri);
			return nullptr;
		}
		SetTextureFilter(rl.texture, TEXTURE_FILTER_POINT);
		return std::make_shared<font>(rl, uri);
	}
};

class raylib_resource_manager final: public resource_manager {
public:
	[[nodiscard]] auto init() -> result<> override;
	[[nodiscard]] auto end() -> result<> override;

	[[nodiscard]] auto load_font(const resource_uri &uri) -> result<font_handle> override;
	[[nodiscard]] auto unload_font(font_handle handle) -> result<> override;
	[[nodiscard]] auto is_font_loaded(font_handle handle) const noexcept -> bool override;
	[[nodiscard]] auto get_raylib_font(font_handle handle) const -> result<Font>;

private:
	using font_cache = entt::resource_cache<font, font_loader>;
	font_cache font_cache_;

	static auto uri_to_key(const resource_uri &uri) noexcept -> entt::id_type {
		return entt::hashed_string{static_cast<std::string>(uri).c_str()}.value();
	}
};

} // namespace lge