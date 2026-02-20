// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/interface/resource_manager.hpp>

#include <entt/core/fwd.hpp>
#include <entt/core/hashed_string.hpp>

namespace lge {

using entt::literals::operator""_hs;

struct previous_sprite {
	sprite_sheet_handle sheet;
	entt::hashed_string frame = ""_hs;
};

} // namespace lge