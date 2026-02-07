// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/result.hpp>

#include "system.hpp"

#include <entity/fwd.hpp>

namespace lge {

class dirty_removal_system: public system {
public:
	explicit dirty_removal_system(const phase p, entt::registry &world): system(p, world) {};
	auto update(float dt) -> result<> override;
};

} // namespace lge