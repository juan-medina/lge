// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/components/placement.hpp>
#include <lge/core/result.hpp>
#include <lge/systems/system.hpp>

#include "movement.hpp"

#include <glm/ext/vector_float2.hpp>

namespace examples {

class movement_system: public lge::system {
public:
	using system::system;

	[[nodiscard]] auto update(float dt) -> lge::result<> override;

private:
	static constexpr auto wall_normal_left = glm::vec2{1.0F, 0.0F};
	static constexpr auto wall_normal_right = glm::vec2{-1.0F, 0.0F};
	static constexpr auto wall_normal_top = glm::vec2{0.0F, 1.0F};
	static constexpr auto wall_normal_bottom = glm::vec2{0.0F, -1.0F};

	static constexpr auto friction = 0.75F;
	static constexpr auto stop_threshold = 5.0F;
	static constexpr auto snap_step = 22.5F;

	[[nodiscard]] auto
	bounce_wall(entt::entity entity, lge::placement &plc, const movement &mov, float half_width, float half_height)
		-> lge::result<>;

	static auto apply_friction(movement &mov, float dt) -> void;
	static auto snap_to_rest(lge::placement &plc, movement &mov) -> void;
};

} // namespace examples