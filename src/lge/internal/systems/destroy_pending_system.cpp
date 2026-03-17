// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "destroy_pending_system.hpp"

#include <lge/components/destroy_pending.hpp>
#include <lge/core/result.hpp>

#include <entity/fwd.hpp>
#include <entt/entt.hpp>
#include <vector>

namespace lge {

auto destroy_pending_system::update(const float /*dt*/) -> result<> {
	for(const auto v = ctx.world.view<destroy_pending>(); const auto e: std::vector<entt::entity>{v.begin(), v.end()}) {
		if(ctx.world.valid(e)) [[likely]] {
			ctx.world.destroy(e);
		}
	}
	return true;
}

} // namespace lge
