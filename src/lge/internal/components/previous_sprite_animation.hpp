// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/interface/resource_manager.hpp>

#include <entt/core/fwd.hpp>
#include <entt/core/hashed_string.hpp>

namespace lge {

using entt::literals::operator""_hs;

struct previous_sprite_animation {
	animation_library_handle handle = invalid_animation_library;
	entt::hashed_string name = ""_hs;
};

} // namespace lge