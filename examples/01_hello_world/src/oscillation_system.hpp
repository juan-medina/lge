// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/effects/oscillator.hpp>
#include <lge/systems/system.hpp>

#include <entity/fwd.hpp>
#include <entt/entt.hpp>

class oscillation_system: public lge::system {
public:
	using system::system;
	auto update(float dt) -> lge::result<> override;

	struct effect {
		lge::oscillator scale;
		lge::oscillator rotation;
	};
};
