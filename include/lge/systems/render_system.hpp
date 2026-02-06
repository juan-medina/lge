// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/renderer.hpp>
#include <lge/result.hpp>

#include "system.hpp"

#include <raylib.h>

namespace lge {

class render_system: public system {
public:
	explicit render_system(entt::registry &world, renderer &renderer): system(world), renderer_{renderer} {}
	auto update(float dt) -> result<> override;
	[[nodiscard]] auto id() const -> system_id override {
		return 1; // Unique ID for the render system
	}

private:
	renderer &renderer_;
};

} // namespace lge