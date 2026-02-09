// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/renderer.hpp>
#include <lge/result.hpp>

#include "system.hpp"

#include <entity/fwd.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float2.hpp>

namespace lge {

class render_system: public system {
public:
	explicit render_system(const phase p, entt::registry &world, renderer &renderer)
		: system(p, world), renderer_{renderer} {}
	auto update(float dt) -> result<> override;

private:
	renderer &renderer_;

	static auto transform_point(const glm::mat3 &m, const glm::vec2 &p) -> glm::vec2;
};

} // namespace lge