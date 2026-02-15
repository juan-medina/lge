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

using resource_id = entt::id_type;
inline constexpr resource_id empty_resource = entt::null;

using font_id = resource_id;

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

	[[nodiscard]] virtual auto load_font(const resource_uri &uri) -> result<font_id> = 0;
	[[nodiscard]] virtual auto unload_font(font_id id) -> result<> = 0;
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
