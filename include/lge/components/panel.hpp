// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/colors.hpp>
#include <lge/interface/resources.hpp>

#include <entt/core/fwd.hpp>
#include <entt/core/hashed_string.hpp>
#include <glm/ext/vector_float2.hpp>

namespace lge {

using entt::literals::operator""_hs;

struct panel {
	sprite_sheet_handle sheet;
	entt::hashed_string frame = ""_hs;
	glm::vec2 size{};
	float border{0.0F};
	color tint = colors::white;
};

} // namespace lge

