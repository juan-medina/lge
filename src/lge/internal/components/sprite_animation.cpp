// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/sprite_animation.hpp>

#include <string>
#include <string_view>

namespace lge {

auto play(sprite_animation &anim, const std::string_view name) -> void {
	anim.current_frame = 0;
	anim.elapsed = 0.F;
	anim.name = name;
}

} // namespace lge