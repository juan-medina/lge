// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/systems/system.hpp>

namespace lge {

class destroy_pending_system: public system {
public:
	using system::system;
	auto update(float dt) -> result<> override;
};

} // namespace lge

