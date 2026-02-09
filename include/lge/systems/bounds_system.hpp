// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/result.hpp>

#include "system.hpp"

#include <entity/fwd.hpp>

namespace lge {

class bounds_system: public system {
public:
	explicit bounds_system(const phase p, entt::registry &world): system(p, world) {}
	auto update(float dt) -> result<> override;
};

} // namespace lge