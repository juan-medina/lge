// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/interface/resource_manager.hpp>

#include <string>
#include <string_view>

namespace lge {

struct sprite_animation {
	animation_library_handle handle;
	std::string name;
	int current_frame = 0;
	float elapsed = 0.F;
};

auto play(sprite_animation &anim, std::string_view name) -> void;

} // namespace lge