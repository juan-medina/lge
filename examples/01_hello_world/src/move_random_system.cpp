// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "move_random_system.hpp"

#include <lge/components/placement.hpp>
#include <lge/result.hpp>

#include <entity/fwd.hpp>
#include <entt/entt.hpp>
#include <random>

auto move_random_system::update(const float dt) -> lge::result<> {
	static std::random_device rd;
	static std::uniform_real_distribution dist(-1.0F, 1.0F);

	for(const auto view = world.view<lge::placement, tag>(); const auto e: view) {
		constexpr float speed = 100.0F;
		auto &pos = view.get<lge::placement>(e).position;
		static std::mt19937 rng(rd());
		pos.x += dist(rng) * dt * speed;
		pos.y += dist(rng) * dt * speed;
	}

	return true;
}
