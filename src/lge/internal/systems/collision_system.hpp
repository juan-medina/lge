// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/events/collision.hpp>
#include <lge/internal/components/bounds.hpp>
#include <lge/systems/system.hpp>

#include <array>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <vector>

namespace lge {

class collision_system: public system {
public:
	using system::system;

	[[nodiscard]] auto update(float dt) -> result<> override;

private:
	std::vector<collision> previous_collisions_;
	std::vector<collision> current_collisions_;

	[[nodiscard]] static auto to_world(const bounds &b, const glm::mat3 &world) noexcept -> std::array<glm::vec2, 4>;

	[[nodiscard]] static auto
	overlaps(const bounds &a, const glm::mat3 &a_world, const bounds &b, const glm::mat3 &b_world) noexcept -> bool;
};

} // namespace lge