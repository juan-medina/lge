// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/result.hpp>

#include "system.hpp"

#include <raylib.h>

#include <glm/glm.hpp>

namespace lge {

class render_system: public system {
public:
	explicit render_system(entt::registry &world): system(world) {}
	auto update(float dt) -> result<> override;
	[[nodiscard]] auto id() const -> system_id override {
		return 1; // Unique ID for the render system
	}

private:
	static Color ColorFromGLM(const glm::vec4 &v) {
		return ColorFromNormalized({v.r, v.g, v.b, v.a});
	}
};

} // namespace lge