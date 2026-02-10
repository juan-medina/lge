// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <entity/fwd.hpp>
#include <entt/entt.hpp>
#include <vector>

namespace lge {

struct parent {
	entt::entity id{};
};

struct children {
	std::vector<entt::entity> ids;
};

inline void attach(entt::registry &world, entt::entity the_parent, entt::entity the_child) {
	world.emplace_or_replace<parent>(the_child, the_parent);
	auto &kids = world.get_or_emplace<children>(the_parent).ids;
	kids.push_back(the_child);
}

} // namespace lge