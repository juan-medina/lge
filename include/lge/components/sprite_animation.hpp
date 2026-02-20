// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/interface/resource_manager.hpp>

#include <string>

namespace lge {

struct sprite_animation {
	animation_library_handle handle;
	std::string name;
	int current_frame = 0;
	float elapsed = 0.F;
	bool flip_horizontal;
	bool flip_vertical;
};

} // namespace lge