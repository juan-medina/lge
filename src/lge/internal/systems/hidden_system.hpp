// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/result.hpp>
#include <lge/systems/system.hpp>

#include <entity/fwd.hpp>
#include <vector>

namespace lge {

class hidden_system: public system {
public:
	explicit hidden_system(const phase p, entt::registry &world): system(p, world) {}
	auto update(float dt) -> result<> override;

private:
	std::vector<entt::entity> hidden_stack_;
};

} // namespace lge