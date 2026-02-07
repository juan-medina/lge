// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once
#include <lge/result.hpp>
#include <lge/systems/system.hpp>

#include <entity/fwd.hpp>
#include <entt/entt.hpp>

class move_random_system: public lge::system {
public:
	explicit move_random_system(const lge::phase p, entt::registry &w): system(p, w) {}
	auto update(float dt) -> lge::result<> override;

	struct tag {};
};
