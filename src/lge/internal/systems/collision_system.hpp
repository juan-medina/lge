// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/events/collision.hpp>
#include <lge/internal/components/bounds.hpp>
#include <lge/systems/system.hpp>

#include <array>
#include <glm/ext/vector_float2.hpp>
#include <vector>

namespace lge {

class collision_system: public system {
public:
	using system::system;

	[[nodiscard]] auto update(float) -> result<> override;

private:
	std::vector<collision> previous_collisions_;
	std::vector<collision> current_collisions_;

	[[nodiscard]] static auto sat_overlap(const std::array<glm::vec2, 3> &tri_a,
										  const std::array<glm::vec2, 3> &tri_b) noexcept -> bool;

	[[nodiscard]] static auto triangles_intersect(const glm::vec2 &a0,
												  const glm::vec2 &a1,
												  const glm::vec2 &a2,
												  const glm::vec2 &b0,
												  const glm::vec2 &b1,
												  const glm::vec2 &b2) noexcept -> bool;

	[[nodiscard]] static auto overlaps(const bounds &a, const bounds &b) noexcept -> bool;
};

} // namespace lge