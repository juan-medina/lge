// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/internal/components/bounds.hpp>
#include <lge/systems/system.hpp>

#include <array>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float2.hpp>

namespace lge {

class pointer_system: public system {
public:
	using system::system;

	[[nodiscard]] auto update(float) -> result<> override;

private:
	[[nodiscard]] static auto
	to_world(const bounds &b, const glm::mat3 &world, const glm::vec2 &pivot, const glm::vec2 &size) noexcept
		-> std::array<glm::vec2, 4>;

	[[nodiscard]] static auto point_in_quad(const glm::vec2 &point, const std::array<glm::vec2, 4> &quad) noexcept
		-> bool;
};

} // namespace lge