// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "transition_system.hpp"

#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>

#include <glm/ext/vector_float2.hpp>

namespace lge {

auto transition_system::update(float /*dt*/) -> result<> {
	const auto alpha = scenes_.overlay_alpha();
	if(alpha == 0) [[likely]] {
		return true;
	}

	const auto resolution = ctx.render.get_drawing_resolution();
	const auto overlay_color = color::from_rgba(0, 0, 0, alpha);

	ctx.render.render_rect({0.0F, 0.0F}, // center (game space origin)
						   resolution,	 // size covers the entire drawing resolution
						   0.0F,		 // no rotation
						   colors::transparent,
						   overlay_color,
						   0.0F);

	return true;
}

} // namespace lge