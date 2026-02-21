// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>
#include <lge/interface/resources.hpp>
#include <lge/internal/resource_manager/resource_store.hpp>

#include <core/fwd.hpp>
#include <entt/entt.hpp>
#include <string_view>
#include <unordered_map>

namespace lge {

class sprite_sheet {
public:
	~sprite_sheet();
	sprite_sheet() = default;
	sprite_sheet(const sprite_sheet &) = delete;
	auto operator=(const sprite_sheet &) -> sprite_sheet & = delete;
	sprite_sheet(sprite_sheet &&) noexcept = default;
	auto operator=(sprite_sheet &&) noexcept -> sprite_sheet & = default;

	[[nodiscard]] auto load(std::string_view uri, resource_manager &rm) -> result<>;
	texture_handle texture;
	std::unordered_map<entt::id_type, sprite_sheet_frame> frames;

private:
	resource_manager *rm_ = nullptr;
};

using sprite_sheet_store = resource_store<sprite_sheet, sprite_sheet_handle>;

} // namespace lge