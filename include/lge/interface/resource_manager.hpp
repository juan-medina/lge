// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>

#include <algorithm>
#include <core/fwd.hpp>
#include <entt/entt.hpp>
#include <filesystem>
#include <format>
#include <string>

namespace lge {

// =============================================================================
// Type-Safe Resource Handles
// =============================================================================

template<typename Tag>
struct resource_handle {
	entt::id_type id = entt::null;

	[[nodiscard]] constexpr auto is_valid() const noexcept -> bool {
		return id != entt::null;
	}

	constexpr auto operator==(const resource_handle &other) const noexcept -> bool = default;

	[[nodiscard]] constexpr auto raw() const noexcept -> entt::id_type {
		return id;
	}

	static constexpr auto from_id(entt::id_type id) noexcept -> resource_handle {
		return resource_handle{id};
	}
};

struct font_tag {};
using font_handle = resource_handle<font_tag>;
inline constexpr font_handle invalid_font{};

struct texture_tag {};
using texture_handle = resource_handle<texture_tag>;
inline constexpr texture_handle invalid_texture{};

struct sprite_sheet_tag {};
using sprite_sheet_handle = resource_handle<sprite_sheet_tag>;
inline constexpr sprite_sheet_handle invalid_sprite_sheet{};

class resource_uri {
public:
	consteval resource_uri(const char *s): value_(s) {}				// NOLINT(*-explicit-constructor)
	consteval resource_uri(const std::string_view &s): value_(s) {} // NOLINT(*-explicit-constructor)

	explicit operator std::string() const {
		return std::string(value_);
	}

	[[nodiscard]] auto exists() const -> bool {
		return std::filesystem::exists(value_);
	}

private:
	std::string_view value_;
};

class resource_manager {
public:
	resource_manager() = default;
	virtual ~resource_manager() = default;

	// Disable copying and moving — resource_manager is not copyable or movable
	resource_manager(const resource_manager &) = delete;
	resource_manager(resource_manager &&) = delete;
	auto operator=(const resource_manager &) -> resource_manager & = delete;
	auto operator=(resource_manager &&) -> resource_manager & = delete;

	[[nodiscard]] virtual auto init() -> result<> = 0;
	[[nodiscard]] virtual auto end() -> result<> = 0;

	[[nodiscard]] virtual auto load_font(const resource_uri &uri) -> result<font_handle> = 0;
	[[nodiscard]] virtual auto unload_font(font_handle handle) -> result<> = 0;
	[[nodiscard]] virtual auto is_font_loaded(font_handle handle) const noexcept -> bool = 0;

	[[nodiscard]] virtual auto load_texture(const resource_uri &uri) -> result<texture_handle> = 0;
	[[nodiscard]] virtual auto unload_texture(texture_handle handle) -> result<> = 0;
	[[nodiscard]] virtual auto is_texture_loaded(texture_handle handle) const noexcept -> bool = 0;

	[[nodiscard]] virtual auto load_sprite_sheet(const resource_uri &uri) -> result<sprite_sheet_handle> = 0;
	[[nodiscard]] virtual auto unload_sprite_sheet(sprite_sheet_handle handle) -> result<> = 0;
	[[nodiscard]] virtual auto is_sprite_sheet_loaded(sprite_sheet_handle handle) const noexcept -> bool = 0;
};

} //  namespace lge

template<>
struct std::formatter<lge::resource_uri> {
	// NOLINTNEXTLINE(*-convert-member-functions-to-static)
	constexpr auto parse(std::format_parse_context &ctx) {
		return ctx.begin();
	}

	// NOLINTNEXTLINE(*-convert-member-functions-to-static)
	auto format(const lge::resource_uri &uri, std::format_context &ctx) const {
		const auto &str = static_cast<const std::string>(uri);
		return std::ranges::copy(str, ctx.out()).out;
	}
};

template<typename Tag>
struct std::formatter<lge::resource_handle<Tag>> {
	constexpr auto parse(std::format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(const lge::resource_handle<Tag> &handle, std::format_context &ctx) const {
		return std::format_to(ctx.out(), "{}", handle.raw());
	}
};