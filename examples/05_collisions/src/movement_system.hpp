// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/systems/system.hpp>

namespace examples {

class movement_system: public lge::system {
public:
	using system::system;

	[[nodiscard]] auto update(float dt) -> lge::result<> override;
};

} // namespace examples
