// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once
#include <lge/result.hpp>
#include <lge/systems/system.hpp>

#include <entity/fwd.hpp>
#include <entt/entt.hpp>

class move_random_system: public lge::system {
public:
	explicit move_random_system(entt::registry &w): system(w) {}

	[[nodiscard]] auto get_phase() const -> lge::phase override {
		return lge::phase::update;
	}

	auto update(float dt) -> lge::result<> override;

	struct tag {};
};
