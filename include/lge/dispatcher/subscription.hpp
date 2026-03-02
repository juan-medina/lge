// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include "entity/entity.hpp"

#include <cstdint>
#include <entt/core/fwd.hpp>
#include <entt/core/type_info.hpp>

namespace lge {

struct subscription {
	entt::id_type event_type = entt::null;
	uint32_t id = 0;

	auto operator==(const subscription &) const -> bool = default;
};

} // namespace lge