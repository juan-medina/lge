// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <entt/entity/entity.hpp>

namespace lge {

struct collision {
	entt::entity first;
	entt::entity second;
};

} // namespace lge