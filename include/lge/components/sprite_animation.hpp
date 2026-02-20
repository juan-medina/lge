// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/interface/resource_manager.hpp>

#include <entt/core/fwd.hpp>
#include <entt/core/hashed_string.hpp>

namespace lge {

using entt::literals::operator""_hs;

struct sprite_animation {
	animation_library_handle handle;
	entt::hashed_string name = ""_hs;
	int current_frame = 0;
	float elapsed = 0.F;
	bool flip_horizontal = false;
	bool flip_vertical = false;
};

} // namespace lge