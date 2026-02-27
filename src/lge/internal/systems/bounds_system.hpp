// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/systems/system.hpp>

#include <entity/fwd.hpp>
#include <glm/fwd.hpp>

namespace lge {

class bounds_system: public system {
public:
	using system::system;
	auto update(float dt) -> result<> override;

private:
	[[nodiscard]] static auto transform_point(const glm::mat3 &m, const glm::vec2 &p) noexcept -> glm::vec2;
	[[nodiscard]] static auto get_scale(const glm::mat3 &m) noexcept -> glm::vec2;
	[[nodiscard]] static auto get_rotation_rad(const glm::mat3 &m) noexcept -> float;
};

} // namespace lge