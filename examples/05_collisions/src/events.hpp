// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <glm/ext/vector_float2.hpp>

namespace examples {

struct dice_hit {
	entt::entity entity{entt::null};
	glm::vec2 normal{};
};

} // namespace examples