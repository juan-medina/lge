// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>
#include <lge/interface/resources.hpp>
#include <lge/internal/resource_manager/resource_store.hpp>

#include <core/fwd.hpp>
#include <entt/entt.hpp>
#include <filesystem>
#include <jsoncons/basic_json.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

namespace lge {

class animation_library {
public:
	~animation_library();
	animation_library() = default;
	animation_library(const animation_library &) = delete;
	auto operator=(const animation_library &) -> animation_library & = delete;
	animation_library(animation_library &&) noexcept = default;
	auto operator=(animation_library &&) noexcept -> animation_library & = default;

	[[nodiscard]] auto load(std::string_view uri, resource_manager &rm) -> result<>;
	sprite_sheet_handle sprite_sheet;
	std::unordered_map<entt::id_type, animation_library_anim> animations;

private:
	resource_manager *rm_ = nullptr;

	auto parse_animations(const jsoncons::json &root) -> result<>;
	static auto parse_sprite_sheet_path(const jsoncons::json &root, const std::filesystem::path &base_path)
		-> result<std::string>;
	static auto parse_animation_library_json(std::string_view uri) -> result<jsoncons::json>;
};

using animation_library_store = resource_store<animation_library, animation_library_handle>;

} // namespace lge