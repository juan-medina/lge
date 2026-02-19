// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include <raylib.h>

#include <entt/core/fwd.hpp>
#include <entt/entt.hpp>
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

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

struct texture {
	Texture2D raylib_texture;
	resource_uri uri;
	explicit texture(const Texture2D &t, const resource_uri &uri): raylib_texture(t), uri(uri) {}
	~texture() {
		UnloadTexture(raylib_texture);
	}
	texture(const texture &) = delete;
	texture(texture &&) = delete;
	auto operator=(const texture &) -> texture & = delete;
	auto operator=(texture &&) -> texture & = delete;
};

struct sprite_sheet_frame {
	glm::vec2 source_pos;
	glm::vec2 source_size;
	glm::vec2 pivot;
};

struct sprite_sheet {
	Texture2D raylib_texture;
	resource_uri uri;
	std::unordered_map<std::string, sprite_sheet_frame> frames;

	explicit sprite_sheet(const Texture2D &t,
						  const resource_uri &u,
						  std::unordered_map<std::string, sprite_sheet_frame> f)
		: raylib_texture(t), uri(u), frames(std::move(f)) {}
	~sprite_sheet() {
		UnloadTexture(raylib_texture);
	}
	sprite_sheet(const sprite_sheet &) = delete;
	sprite_sheet(sprite_sheet &&) = delete;
	auto operator=(const sprite_sheet &) -> sprite_sheet & = delete;
	auto operator=(sprite_sheet &&) -> sprite_sheet & = delete;
};

struct texture_loader: entt::resource_loader<texture> {
	using result_type = std::shared_ptr<texture>;

	auto operator()(const resource_uri &uri) const -> result_type {
		const auto rl = LoadTexture(std::string(uri).c_str());
		if(rl.id == 0) [[unlikely]] {
			log::error("failed to load texture from uri: {}", uri);
			return nullptr;
		}
		SetTextureFilter(rl, TEXTURE_FILTER_POINT);
		return std::make_shared<texture>(rl, uri);
	}
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

struct sprite_sheet_loader: entt::resource_loader<sprite_sheet> {
	using result_type = std::shared_ptr<sprite_sheet>;
	auto operator()(const resource_uri &uri) const -> result_type;
};

class raylib_resource_manager final: public resource_manager {
public:
	[[nodiscard]] auto init() -> result<> override;
	[[nodiscard]] auto end() -> result<> override;

	[[nodiscard]] auto load_font(const resource_uri &uri) -> result<font_handle> override;
	[[nodiscard]] auto unload_font(font_handle handle) -> result<> override;
	[[nodiscard]] auto is_font_loaded(font_handle handle) const noexcept -> bool override;
	[[nodiscard]] auto get_raylib_font(font_handle handle) const -> result<Font>;

	[[nodiscard]] auto load_texture(const resource_uri &uri) -> result<texture_handle> override;
	[[nodiscard]] auto unload_texture(texture_handle handle) -> result<> override;
	[[nodiscard]] auto is_texture_loaded(texture_handle handle) const noexcept -> bool override;
	[[nodiscard]] auto get_raylib_texture(texture_handle handle) const -> result<Texture2D>;

	[[nodiscard]] auto load_sprite_sheet(const resource_uri &uri) -> result<sprite_sheet_handle> override;
	[[nodiscard]] auto unload_sprite_sheet(sprite_sheet_handle handle) -> result<> override;
	[[nodiscard]] auto is_sprite_sheet_loaded(sprite_sheet_handle handle) const noexcept -> bool override;
	[[nodiscard]] auto get_sprite_sheet_frame(sprite_sheet_handle handle, std::string_view frame_name) const
		-> result<sprite_sheet_frame>;
	[[nodiscard]] auto get_sprite_sheet_texture(sprite_sheet_handle handle) const -> result<Texture2D>;

private:
	using font_cache = entt::resource_cache<font, font_loader>;
	using texture_cache = entt::resource_cache<texture, texture_loader>;
	using sprite_sheet_cache = entt::resource_cache<sprite_sheet, sprite_sheet_loader>;
	font_cache font_cache_;
	texture_cache texture_cache_;
	sprite_sheet_cache sprite_sheet_cache_;

	static auto uri_to_key(const resource_uri &uri) noexcept -> entt::id_type {
		return entt::hashed_string{static_cast<std::string>(uri).c_str()}.value();
	}
};

} // namespace lge