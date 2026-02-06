// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/result.hpp>

#include "system.hpp"

namespace lge {

class render_system: public system {
public:
	explicit render_system(entt::registry &world): system(world) {}
	auto update(float dt) -> result<> override;
	[[nodiscard]] auto id() const -> system_id override {
		return 1; // Unique ID for the render system
	}
};

} // namespace lge