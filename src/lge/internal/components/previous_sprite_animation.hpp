// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/interface/resource_manager.hpp>

#include <string>

namespace lge {

struct previous_sprite_animation {
	animation_library_handle handle = invalid_animation_library;
	std::string name;
};


} // namespace lge