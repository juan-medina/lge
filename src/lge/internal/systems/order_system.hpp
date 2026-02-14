// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/result.hpp>
#include <lge/systems/system.hpp>

#include <entity/fwd.hpp>
#include <vector>

namespace lge {

class order_system: public system {
public:
	explicit order_system(phase p, entt::registry &world): system(p, world) {}
	auto update(float dt) -> result<> override;

private:
	std::vector<entt::entity> order_stack_;
};

} // namespace lge