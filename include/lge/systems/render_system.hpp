// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/renderer.hpp>
#include <lge/result.hpp>

#include "system.hpp"

#include <entt/entt.hpp>

namespace lge {

class render_system: public system {
public:
	explicit render_system(entt::registry &world, renderer &renderer): system(world), renderer_{renderer} {}
	auto update(float dt) -> result<> override;

	[[nodiscard]] auto get_phase() const -> phase override {
		return phase::render;
	}

private:
	renderer &renderer_;
};

} // namespace lge