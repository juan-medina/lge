// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/systems/system.hpp>

#include <entity/fwd.hpp>

namespace lge {

class bounds_system: public system {
public:
	using system::system;
	auto update(float dt) -> result<> override;
};

} // namespace lge